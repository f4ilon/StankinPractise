#include "Client.h"


Client::Client() {
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

void Client::sendMessage(std::string data) {
    Message message;
    message.fromUser = name;
    message.type = "standardMessage";
    message.message = data;
    std::string packedMessage = pack(message);
    std::cout << "You: " << data << "\n";
    send(Socket, packedMessage.c_str(), packedMessage.length(), 0);
}

void Client::getMessage() {
    while (true) {
        char buffer[1024] = {0};
        int bytesReceived = recv(Socket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            Message inputMessage = unpack(buffer);

            // Формируем красивую строку: "Имя: Сообщение"
            std::string formattedMessage = inputMessage.fromUser + ": " + inputMessage.message;

            // Передаем строку в интерфейс
            if (onMessageReceived) {
                onMessageReceived(formattedMessage);
            }
        } else if (bytesReceived <= 0) {
            if (onMessageReceived) {
                onMessageReceived("[Система]: Сервер отключился.");
            }
            break;
        }
    }
}
