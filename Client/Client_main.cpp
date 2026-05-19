#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
#include "Client.h"
#include "Utils.h"

#ifdef _WIN32
#include <windows.h>
#include <queue> // Для буферизации UTF-8 символов в Windows
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#endif

std::atomic<bool> keepRunning{ true };
std::mutex ui_mtx;
std::vector<std::string> messages;
std::string current_input;
Client client;

// ====================== WINDOWS INPUT ======================
#ifdef _WIN32
std::queue<char> win_input_buffer;

bool win_kbhit() {
    if (!win_input_buffer.empty()) return true;

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD eventsAvail = 0;
    GetNumberOfConsoleInputEvents(hIn, &eventsAvail);
    if (eventsAvail == 0) return false;

    INPUT_RECORD ir;
    DWORD count;
    ReadConsoleInputW(hIn, &ir, 1, &count);

    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
        WORD vk = ir.Event.KeyEvent.wVirtualKeyCode;
        WCHAR wch = ir.Event.KeyEvent.uChar.UnicodeChar;

        if (vk == VK_RETURN) { win_input_buffer.push('\n'); return true; }
        if (vk == VK_BACK) { win_input_buffer.push(8); return true; }
        if (vk == VK_ESCAPE) { win_input_buffer.push(27); return true; }

        if (wch >= 32) { // Любой печатаемый символ
            std::string utf8 = WCharToString(wch); // Конвертируем в UTF-8
            for (char c : utf8) {
                win_input_buffer.push(c); // Разбиваем на байты и кладем в очередь
            }
            return true;
        }
    }
    return false;
}

char win_getch() {
    if (win_input_buffer.empty()) return 0;
    char c = win_input_buffer.front();
    win_input_buffer.pop();
    return c;
}
#endif

// ====================== LINUX INPUT ======================
#ifndef _WIN32
struct termios orig_termios;

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void init_terminal_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(reset_terminal_mode);
    struct termios new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

bool linux_kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

char linux_getch() {
    char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) < 0) return 0;
    return ch;
}
#endif
// =========================================================

void draw_ui() {
    std::lock_guard<std::mutex> lock(ui_mtx);

#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Возвращаем курсор в начало видимого окна (исправляет наслоение)
    COORD coord;
    coord.X = csbi.srWindow.Left;
    coord.Y = csbi.srWindow.Top;
    SetConsoleCursorPosition(hOut, coord);
#else
    std::cout << "\033[2J\033[H";
    int width = 80;
    int height = 24;
#endif

    int chat_height = height - 3;
    int start_idx = (std::max)(0, (int)messages.size() - chat_height);

    for (int i = 0; i < chat_height; ++i) {
        if (start_idx + i < (int)messages.size()) {
            std::string msg = messages[start_idx + i];
            int pad = (std::max)(0, width - utf8_visible_length(msg) - 1);
            std::cout << msg << std::string(pad, ' ') << "\n"; // Добиваем пробелами
        }
        else {
            std::cout << std::string(width - 1, ' ') << "\n"; // Пустые строки
        }
    }

    std::string room_str = client.room;
    int pad_room = (std::max)(0, width - utf8_visible_length(room_str) - 1);
    std::cout << room_str << std::string(pad_room, '-') << "\n";

    std::string prompt = "You: " + current_input;
    int pad_prompt = (std::max)(0, width - utf8_visible_length(prompt) - 1);

    // Трюк с \r: печатаем строку, забиваем хвост пробелами, возвращаем каретку и печатаем строку снова.
    // Это оставит мигающий курсор ровно в конце введенного текста!
    std::cout << prompt << std::string(pad_prompt, ' ') << "\r" << prompt << std::flush;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    init_terminal_mode();
#endif

    std::cout << "Connecting to server...\n";

    while (keepRunning) {
        if (client.tryConnect()) break;
        std::cerr << "Connection failed. Reconnecting in 2 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

#ifdef _WIN32
    system("cls"); // Единоразово чистим консоль после подключения на винде
#endif

    std::cout << "\n=== Подключено! ===\n";

    client.onMessageReceived = [](const std::string& text) {
        {
            std::lock_guard<std::mutex> lock(ui_mtx);
            messages.push_back(text);
        }
        draw_ui();
        };

    std::thread receive_thread(&Client::getMessage, &client);
    receive_thread.detach();

    draw_ui();

    // ==================== Основной цикл ввода ====================
    while (keepRunning) {
        bool ui_needs_update = false;

        while (true) {
            char ch = 0;

#ifdef _WIN32
            if (win_kbhit()) ch = win_getch();
            else break;
#else
            if (linux_kbhit()) ch = linux_getch();
            else break;
#endif

            if (ch == '\n' || ch == '\r') {  // Enter
                if (!current_input.empty()) {
                    std::string msg = current_input;
                    current_input.clear();
                    
                    // Отправляем, только если есть хотя бы один нормальный символ
                    if (msg.find_first_not_of(" \t\r\n") != std::string::npos) {
                        client.sendMessage(msg);
                        // Добавляем себе в историю только если отправили
                        messages.push_back("You: " + msg);         
                    }
                }
            }
            else if (ch == 8 || ch == 127) { // Backspace
                if (!current_input.empty()) {
                    while (!current_input.empty() &&
                        (unsigned char)current_input.back() >= 0x80 &&
                        (unsigned char)current_input.back() <= 0xBF) {
                        current_input.pop_back();
                    }
                    if (!current_input.empty()) current_input.pop_back();
                }
            }
            else if (ch == 27) { // ESC
#ifndef _WIN32
                if (linux_kbhit()) {
                    while (linux_kbhit()) linux_getch();
                }
                else {
                    keepRunning = false;
                }
#else
                keepRunning = false;
#endif
            }
            else if ((unsigned char)ch >= 32) {
                // Если это не тильда, добавляем в строку ввода
                if (ch != '~') { 
                    current_input += ch;
                }
            }

            ui_needs_update = true;
        }

        if (ui_needs_update) {
            draw_ui();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    client.stop();

#ifdef _WIN32
    system("cls");
    WSACleanup();
#endif

    std::cout << "\nКлиент завершил работу.\n";
    return 0;
}