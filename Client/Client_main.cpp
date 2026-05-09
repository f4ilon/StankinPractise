#include <iostream>
#include <thread>
#include <chrono>
#include "Client.h"

int main() {
    std::cout << "Запуск клиента...\n";

    Client client;
    if (!client.tryConnect()) {
        std::cerr << "Не удалось подключиться к серверу.\n";
        return 1;
    }

    std::cout << "Подключено! Введите /help для команд.\n";

    client.onMessageReceived = [](const std::string& text) {
        std::cout << "\r" << text << "\n> " << std::flush;
    };

    std::thread receive_thread(&Client::getMessage, &client);
    receive_thread.detach();

    std::string input;
    while (true) {
        std::cout << "> " << std::flush;
        std::getline(std::cin, input);

        if (input == "/exit") break;
        if (!input.empty()) {
            client.sendMessage(input);
        }
    }

    client.stop();
    return 0;
}
