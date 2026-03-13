#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>


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
    void pack(std::string message) {
        std::string data = name + ": " + message;
        char data_c[std::size(data)];
        for (int i = 0; i < data.length(); i++) {
            data_c[i] = data[i];
        }
        std::cout << "You: " << message;
        send(Socket, data_c, sizeof(data_c), 0);
    }

    // void unpack(char data[]) {
    //     std::string message;
    //     for (int i = 0; i < sizeof(data); i++) {
    //         message += data[i];
    //     }
    //     std::cout << message << "\n";
    // }

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