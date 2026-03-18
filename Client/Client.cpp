#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "../Common/Packets.hpp"


class Client {
    public:
    SOCKET Socket;
    sockaddr_in serverAddr{};
    std::string name;
    Client() {
        // Инициализация Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed.\n";
        }
        // Создание сокета
        Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (Socket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << "\n";
            WSACleanup();
        }
        // Настройка адреса сервера
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        // Указываем IP-адрес сервера
        InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
        std::cout << "Enter nickname: ";
        std::cin >> name;
    }

    void sendMessage(std::string data) {
        Message message;
        message.fromUser = name;
        message.type = "standardMessage";
        message.message = data;
        char* data_c = pack(message);
        std::cout << "You: " << data << "\n";
        std::cout << data_c << "\n";
        send(Socket, data_c, sizeof(data_c), 0);
    }

    void getMessage() {
        while (true) {
            char buffer[1024] = {0};
            int bytesReceived = recv(Socket, buffer, std::size(buffer), 0);
            if (bytesReceived > 0) {
                std::cout << buffer << "\n";
                std::cout << "> " << std::flush;
            }
        }

    }
};