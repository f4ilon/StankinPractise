#ifndef CMESS_PACKETS_HPP
#define CMESS_PACKETS_HPP


#include <string>
#include <sstream>

struct Message {
    std::string type;
    std::string fromUser;
    std::string message;
};

std::string pack(Message data);
Message unpack(char* pack_data);


#endif //CMESS_PACKETS_HPP