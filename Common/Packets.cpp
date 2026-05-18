#include "Packets.h"
#include <sstream>
#include <iostream>

std::string pack(const Message& data) {
    return data.type + "~" + data.fromUser + "~" + data.message + "~";
}

std::string pack(const std::string& type, const std::string& fromUser, const std::string& message) {
    return type + "~" + fromUser + "~" + message + "~";
}

Message unpack(const std::string& pack_data) {
    Message data;
    std::stringstream ss(pack_data);
    
    std::getline(ss, data.type, '~');
    std::getline(ss, data.fromUser, '~');
    std::getline(ss, data.message, '~');

    // Защита от повреждённых пакетов
    if (data.type.empty()) {
        data.type = "error";
        data.message = "Invalid packet";
    }

    return data;
}