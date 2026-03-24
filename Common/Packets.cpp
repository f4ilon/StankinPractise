#include "Packets.h"

#include <iostream>

Message unpack(char* pack_data) {
    Message data;
    std::stringstream ss(pack_data);

    std::getline(ss, data.type, '~');
    std::getline(ss, data.fromUser, '~');
    std::getline(ss, data.message, '~');

    return data;
}

std::string pack(Message data) {
    return data.type + "~" + data.fromUser + "~" + data.message + "~";
}