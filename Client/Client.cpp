#include "Client.h"
#include <cstring>   // для memset

Client::Client() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }
#endif

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);   // кроссплатформенная версия

    std::cout << "Enter nickname: ";
    std::cin >> name;
    std::cin.ignore(); // очистка буфера на случай getline позже
}

bool Client::tryConnect() {
    int result = connect(Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result != 0) {
#ifdef _WIN32
        std::cerr << "[Client] Connection failed: " << WSAGetLastError() << "\n";
#else
        std::cerr << "[Client] Connection failed: " << strerror(errno) << "\n";
#endif
        return false;
    }
    return true;
}

void Client::sendMessage(const std::string& data) {
    Message message;
    message.type = "standardMessage";
    message.fromUser = name;
    message.message = data;

    std::string packedMessage = pack(message);
    std::cout << "You: " << data << "\n";
    
    int bytes = send(Socket, packedMessage.c_str(), static_cast<int>(packedMessage.length()), 0);
    if (bytes < 0) {
#ifdef _WIN32
        std::cerr << "[Client] Send failed: " << WSAGetLastError() << "\n";
#else
        std::cerr << "[Client] Send failed: " << strerror(errno) << "\n";
#endif
        stop();
    }
}

void Client::getMessage() {
    while (true) {
        char buffer[1024] = {0};
        int bytesReceived = recv(Socket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            Message inputMessage = unpack(buffer);

            if (inputMessage.type == "standardMessage") {
                std::string formattedMessage = inputMessage.fromUser + ": " + inputMessage.message;
                if (onMessageReceived) {
                    onMessageReceived(formattedMessage);
                }
            }
            else if (inputMessage.type == "changeRoom") {
                room = inputMessage.message;
            }
        } 
        else {
            if (onMessageReceived) {
                onMessageReceived("[Система]: Сервер отключился.");
            }
            break;
        }
    }
}

void Client::stop() {
    if (Socket != INVALID_SOCKET) {
#ifndef _WIN32
        shutdown(Socket, SHUT_RDWR);
#endif

        CLOSE_SOCKET(Socket);
#ifdef _WIN32
        WSACleanup();
#endif
    }
}

Client::Client(const std::string& nickname, short port) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    
    name = nickname;
}