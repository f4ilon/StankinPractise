#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "Server.cpp"
#include <vector>
#include <algorithm>
#include "ThreadPool.cpp"


bool keepRunning(true);
ThreadPool pool(8);


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
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    Server server;
    SOCKET serverSocket = server.Socket;


    // Обработка подключений
    while (keepRunning) {
        sockaddr_in clientAddr{};// 6. Ожидание подключения клиента
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
        auto result{ std::find(begin(ClientHandler), end(ClientHandler), clientSocket) };
        if (result == end(ClientHandler)) {
            ClientHandler.push_back(clientSocket);
        }
        pool.enqueue([clientSocket]() {handleClient(clientSocket);});
    }

    // Закрытие серверного сокета и очистка
    closesocket(serverSocket);
    WSACleanup();
    std::cout << "Press \"Enter\" to exit\n";
    std::string ex;
    std::cin>>ex;
    return 0;
}