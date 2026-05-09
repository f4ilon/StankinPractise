#include "Client.h"

Client::Client() {
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket\n";
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    std::cout << "Enter nickname: ";
    std::cin >> name;
}

bool Client::tryConnect() {
    return connect(Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0;
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
            if (inputMessage.type == "standardMessage") {
                std::string formattedMessage = inputMessage.fromUser + ": " + inputMessage.message;
                if (onMessageReceived) onMessageReceived(formattedMessage);
            }
            else if (inputMessage.type == "changeRoom") {
                room = inputMessage.message;
            }
        } else {
            if (onMessageReceived) onMessageReceived("[Система]: Сервер отключился.");
            break;
        }
    }
}

void Client::stop() {
    CLOSE_SOCKET(Socket);
}
