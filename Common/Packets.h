#pragma once

#include <string>

struct Message {
    std::string type;
    std::string fromUser;
    std::string message;
};

std::string pack(const Message& data);
Message unpack(const std::string& pack_data);

// Для удобства
std::string pack(const std::string& type, const std::string& fromUser, const std::string& message);