#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <windows.h>


std::vector<SOCKET> ClientHandler;
void send2All(SOCKET senderSocket, char message[]);


void handleClient(int clientSocket) {

    if (clientSocket == INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    std::cout << "Client"<<clientSocket<<" connected!"<<"\n";
    // Обмен данными
    while (clientSocket) {
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            //std::cout << "Received from client"<<clientSocket<<": " << buffer << "\n";
            std::cout << buffer << "\n";
            // Отправка ответа
            //char response[50] = "";
            //int responseLength = sizeof(response);
            //send(clientSocket, response, responseLength, 0);
            send2All(clientSocket, buffer);
        }
        else {
            std::cout << "Client"<<clientSocket<<" disconnected!" <<"\n";
            closesocket(clientSocket);
            break;
        }
    }
}


void send2All(SOCKET senderSocket, char message[]) {
    int messageLength = strlen(message);
    for (SOCKET receiverSocket: ClientHandler) {
        if (receiverSocket != senderSocket) send(receiverSocket, message, messageLength, 0);
    }
}


class Server {
public:
    SOCKET Socket;
    sockaddr_in serverAddr{};
    Server() {
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
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        // Привязка сокета к адресу
        if (bind(Socket, (reinterpret_cast<SOCKADDR*>(&serverAddr)), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
            closesocket(Socket);
            WSACleanup();
            //return 3; TODO вывод ошибки
        }
        // Перевод сокета в режим прослушивания
        if (listen(Socket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
            closesocket(Socket);
            WSACleanup();
            //return 4; TODO вывод ошибки
        }
        std::cout << "Server is listening on port 8080...\n";
    }
};