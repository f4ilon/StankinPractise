#include "Server.h"
#include <iostream>
#include <boost/asio.hpp>

int main() {
    try {
        std::cout << "=== Cmess Server ===\n";
        std::cout << "Запуск сервера на порту 8080...\n";
        std::cout << "Для остановки нажмите Ctrl+C\n\n";

        boost::asio::io_context io_context;
        Server server(io_context, 8080);

        io_context.run();  // Блокирующий вызов — сервер работает
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка!\n";
    }

    return 0;
}