#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
#include <string>
#include <windows.h>
#include "Client.cpp"




bool keepRunning(true);


// Функция-обработчик "прерывания"
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        std::cout << "Signal catch\n";
        keepRunning = false;
        return TRUE;
    }
    return FALSE;
}




int main() {
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    Client client;
    SOCKET clientSocket = client.Socket;
    sockaddr_in serverAddr = client.serverAddr;

    while (keepRunning){
        // Подключение к серверу
        bool success = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        while (success) {
            if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "Connection failed: " << WSAGetLastError() << "\n";
                std::cout << "Reconnecting...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            success = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        }
        std::cout << "Connected to server!\n";

        // Запускаем поток для чтения сообщений
        std::thread receive_thread([&client]() {
            client.getMessage();
        });
        receive_thread.detach();

        // Отправка и
        while (!success) {
            std::string message;
            std::getline(std::cin, message);
            std::cout << "\033[1A" << "\033[2K" << "\r";
            if (message.empty() || message.find_first_not_of(' ') == std::string::npos) continue;;
            client.pack(message);
        }
    }

    // Закрытие сокета и очистка
    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Press \"Enter\" to exit\n";
    std::string aw;
    std::cin>>aw;
    return 0;
}