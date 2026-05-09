#include "Server.h"
#include <iostream>

int main() {
    try {
        std::cout << "Запуск сервера на порту 8080...\n";
        boost::asio::io_context io_context;
        Server server(io_context, 8080);
        io_context.run(); 
    } 
    catch (std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
    }
    return 0;
}
