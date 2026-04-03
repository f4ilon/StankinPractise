#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <windows.h>
#include <conio.h>
#include "Utils.h"
#include "Client.h"

std::atomic<bool> keepRunning = true;
std::mutex ui_mtx;
std::vector<std::string> messages;
std::string current_input;

HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

Client client;

// Отрисовка истории и ввода
void draw_ui() {
    std::lock_guard<std::mutex> lock(ui_mtx);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int chat_height = height - 2;

    set_cursor(0, 0);
    int start_idx = (int)messages.size() > chat_height
                    ? (int)messages.size() - chat_height : 0;

    for (int i = 0; i < chat_height; ++i) {
        if (start_idx + i < (int)messages.size()) {
            std::string msg = messages[start_idx + i];
            int visible = utf8_visible_length(msg);
            int pad = std::max(0, width - visible - 1);
            std::cout << msg << std::string(pad, ' ') << "\n";
        } else {
            std::cout << std::string(width - 1, ' ') << "\n";
        }
    }
    set_cursor(0, height - 2);
    std::cout << client.room << std::string(width - 1 - utf8_visible_length(client.room), '-') << "\n";
    set_cursor(0, height - 1);
    std::string prompt = "You: " + current_input;
    int pad = std::max(0, width - utf8_visible_length(prompt) - 1);
    std::cout << prompt << std::string(pad, ' ');
    set_cursor(utf8_visible_length(prompt), height - 1);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut, &cci);
    cci.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cci);



    std::cout << "Connecting to server...\n";
    while (keepRunning) {
        if (client.tryConnect()) break;
        std::cerr << "Connection failed. Reconnecting...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    system("cls");
    { std::lock_guard<std::mutex> lock(ui_mtx); messages.push_back("[Система]: Подключено!"); }
    draw_ui();

    client.onMessageReceived = [](const std::string& text) {
        {
            std::lock_guard<std::mutex> lock(ui_mtx);
            messages.push_back(text);
        }
        draw_ui();
    };

    std::thread receive_thread([]() {
            client.getMessage();
        });
    receive_thread.detach();

    while (keepRunning) {
        INPUT_RECORD ir;
        DWORD count;

        DWORD eventsAvail = 0;
        GetNumberOfConsoleInputEvents(hIn, &eventsAvail);
        if (eventsAvail == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        ReadConsoleInputW(hIn, &ir, 1, &count);

        // Обрабатываем только KEY_EVENT при нажатии
        if (ir.EventType != KEY_EVENT || !ir.Event.KeyEvent.bKeyDown)
            continue;

        WORD vk  = ir.Event.KeyEvent.wVirtualKeyCode;
        wchar_t wch = ir.Event.KeyEvent.uChar.UnicodeChar;

        if (vk == VK_RETURN) {                     // Enter
            if (!current_input.empty() &&
                current_input.find_first_not_of(' ') != std::string::npos)
            {
                std::string msg = current_input;
                { std::lock_guard<std::mutex> lk(ui_mtx); current_input.clear(); }
                client.sendMessage(msg);
                { std::lock_guard<std::mutex> lk(ui_mtx); messages.push_back("You: " + msg); }
            }
        }
        else if (vk == VK_BACK) {                  // Backspace
            std::lock_guard<std::mutex> lk(ui_mtx);
            if (!current_input.empty()) {
                // UTF-8: удаляем хвостовые байты-продолжения
                while (!current_input.empty() &&
                       (unsigned char)current_input.back() >= 0x80 &&
                       (unsigned char)current_input.back() <= 0xBF)
                    current_input.pop_back();
                if (!current_input.empty()) current_input.pop_back();
            }
        }
        else if (vk == VK_ESCAPE) {                // ESC
            keepRunning = false;
            break;
        }
        else if (wch >= 0x20) {                   // Любой печатаемый символ
            std::lock_guard<std::mutex> lk(ui_mtx);
            current_input += WCharToString(wch);  // конвертируем в String
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        draw_ui();
    }

    system("cls");
    client.stop();
    if (receive_thread.joinable()) receive_thread.join();
    WSACleanup();
    return 0;
}