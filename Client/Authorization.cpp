#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

class AuthSystem {
private:
    // Хранилище: Логин -> Хеш пароля
    std::unordered_map<std::string, size_t> users_db;

    // Вспомогательная функция для хеширования пароля
    size_t hashPassword(const std::string& password) {
        return std::hash<std::string>{}(password);
    }

public:
    // Регистрация нового пользователя
    bool registerUser(const std::string& username, const std::string& password) {
        if (users_db.find(username) != users_db.end()) {
            std::cout << "Ошибка: Пользователь " << username << " уже существует.\n";
            return false;
        }

        users_db[username] = hashPassword(password);
        std::cout << "Успех: Пользователь " << username << " зарегистрирован.\n";
        return true;
    }

    // Авторизация пользователя
    bool loginUser(const std::string& username, const std::string& password) {
        auto it = users_db.find(username);
        
        if (it == users_db.end()) {
            std::cout << "Ошибка: Пользователь не найден.\n";
            return false;
        }

        if (it->second == hashPassword(password)) {
            std::cout << "Успех: Добро пожаловать, " << username << "!\n";
            return true;
        } else {
            std::cout << "Ошибка: Неверный пароль.\n";
            return false;
        }
    }
};

// Пример использования
int main() {
    AuthSystem auth;


    auth.registerUser("alice", "super_secret123");
    auth.registerUser("bob", "qwerty");
    auth.registerUser("alice", "another_password");

    std::cout << "-------------------\n";

    // Тестируем вход
    auth.loginUser("alice", "super_secret123");
    auth.loginUser("alice", "wrong_password");
    auth.loginUser("charlie", "password");

    return 0;
}
