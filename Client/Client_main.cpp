#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <windows.h>
#include <conio.h>
#include "Client.h"

// Глобальные переменные для интерфейса
bool keepRunning = true;
std::mutex ui_mtx;                 // Защита консоли от одновременной записи
std::vector<std::string> messages; // История чата
std::string current_input = "";    // То, что пользователь сейчас набирает

// --- ФУНКЦИИ ИНТЕРФЕЙСА ---

// Установка курсора в консоли
void set_cursor(short x, short y) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x, y});
}

// Отрисовка разделенного экрана
void draw_ui() {
    std::lock_guard<std::mutex> lock(ui_mtx);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    int chat_height = height - 2;

    // Отрисовка истории (верхняя часть)
    set_cursor(0, 0);
    int start_idx = 0;
    if (messages.size() > chat_height) {
        start_idx = messages.size() - chat_height;
    }

    for (int i = 0; i < chat_height; ++i) {
        if (start_idx + i < messages.size()) {
            std::string msg = messages[start_idx + i];
            std::cout << msg << std::string(width - msg.length() - 1, ' ') << "\n";
        } else {
            std::cout << std::string(width - 1, ' ') << "\n";
        }
    }

    // Разделитель
    set_cursor(0, height - 2);
    std::cout << std::string(width - 1, '-') << "\n";

    // Поле ввода
    set_cursor(0, height - 1);
    std::string prompt = "You: " + current_input;
    std::cout << prompt << std::string(width - prompt.length() - 1, ' ');

    // Возвращаем курсор к тексту
    set_cursor(prompt.length(), height - 1);
}

// --- ОСНОВНАЯ ПРОГРАММА ---

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Скрываем скроллбар терминала (опционально)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hOut, &cci);
    cci.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cci);

    Client client;
    SOCKET clientSocket = client.Socket;
    sockaddr_in serverAddr = client.serverAddr;

    // 1. ЦИКЛ ПОДКЛЮЧЕНИЯ
    std::cout << "Connecting to server...\n";
    while (keepRunning) {
        int result = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Connection failed: " << WSAGetLastError() << "\n";
            std::cout << "Reconnecting...\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            break; // Успешно подключились! Выходим из цикла.
        }
    }

    // Очищаем экран перед запуском чата
    system("cls");
    messages.push_back("[Система]: Подключено к серверу!");
    draw_ui();

    // 2. ЗАПУСК ПОТОКА ЧТЕНИЯ СЕРВЕРА
    client.onMessageReceived = [](const std::string& text) {
        {
            std::lock_guard<std::mutex> lock(ui_mtx);
            messages.push_back(text);
        }
        draw_ui();
    };

    std::thread receive_thread(&Client::getMessage, &client);
    receive_thread.detach();

    // 3. ЦИКЛ ВВОДА И ОТПРАВКИ
    while (keepRunning) {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r') { // Нажат Enter
                if (!current_input.empty() && current_input.find_first_not_of(' ') != std::string::npos) {

                    // Отправляем сообщение на сервер
                    client.sendMessage(current_input);

                    // Добавляем свое сообщение на экран локально
                    {
                        std::lock_guard<std::mutex> lock(ui_mtx);
                        messages.push_back("You: " + current_input);
                        current_input.clear();
                    }
                }
            }
            else if (ch == '\b') { // Нажат Backspace
                std::lock_guard<std::mutex> lock(ui_mtx);
                if (!current_input.empty()) {
                    // Хак для UTF-8: русская буква занимает 2 байта, английская 1 байт.
                    // Если символ < 0, значит это многобайтовый символ UTF-8.
                    if (current_input.back() < 0) {
                        current_input.pop_back();
                        if (!current_input.empty()) current_input.pop_back();
                    } else {
                        current_input.pop_back();
                    }
                }
            }
            else if (ch == 27) { // Нажат ESC
                keepRunning = false;
                break;
            }
            else if (ch >= 32 || ch < 0) { // Печать обычных символов
                std::lock_guard<std::mutex> lock(ui_mtx);
                current_input += ch;
            }

            draw_ui(); // Обновляем экран при любом нажатии
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Не грузим процессор
    }

    // Закрытие сокета и очистка
    system("cls");
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Disconnected. Press \"Enter\" to exit\n";
    std::string aw;
    std::getline(std::cin, aw);
    return 0;
}