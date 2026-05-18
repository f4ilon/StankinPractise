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
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <sys/select.h>
#endif

std::atomic<bool> keepRunning{true};
std::mutex ui_mtx;
std::vector<std::string> messages;
std::string current_input;
Client client;

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
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    std::cout << "\033[2J\033[H";  
    int width = 80;  
    int height = 24;
#endif

    int chat_height = height - 3;
    int start_idx = std::max(0, (int)messages.size() - chat_height);

    for (int i = 0; i < chat_height; ++i) {
        if (start_idx + i < (int)messages.size()) {
            std::cout << messages[start_idx + i] << "\n";
        } else {
            std::cout << "\n";
        }
    }

    std::cout << client.room << std::string(std::max(0, width - utf8_visible_length(client.room) - 1), '-') << "\n";
    std::cout << "You: " << current_input << std::flush;
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

        // Внутренний цикл: вычитываем ВСЕ нажатые клавиши и байты из буфера ОС
        // до тех пор, пока буфер не опустеет. 
        while (true) {
            char ch = 0;
            
#ifdef _WIN32
            if (_kbhit()) ch = _getch();
            else break; // Выходим из внутреннего цикла, если клавиш больше нет
#else
            if (linux_kbhit()) ch = linux_getch();
            else break; // Выходим из внутреннего цикла, если клавиш больше нет
#endif

            if (ch == '\n' || ch == '\r') {  // Enter
                if (!current_input.empty()) {
                    std::string msg = current_input;
                    current_input.clear();
                    client.sendMessage(msg);
                    messages.push_back("You: " + msg);
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
                    // Это была спец-клавиша (стрелка и т.д.), съедаем мусор
                    while (linux_kbhit()) linux_getch();
                } else {
                    keepRunning = false; // Честное нажатие ESC
                }
#else
                keepRunning = false;
#endif
            }
            else if ((unsigned char)ch >= 32) {
                current_input += ch;
            }
            
            ui_needs_update = true;
        }

        // Вызываем отрисовку ТОЛЬКО когда прочитали все символы
        // Это гарантирует, что мы не сломаем терминал оборванным UTF-8 символом
        if (ui_needs_update) {
            draw_ui();
        }

        // Спим 10 мс, чтобы не грузить процессор на 100%
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    client.stop();

#ifdef _WIN32
    WSACleanup();
#endif

    std::cout << "\nКлиент завершил работу.\n";
    return 0;
}