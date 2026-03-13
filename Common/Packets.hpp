#ifndef CMESS_PACKETS_HPP
#define CMESS_PACKETS_HPP


#include <string>

struct Message {
    std::string type;
    std::string fromUser;
    std::string message;
};

char* pack(Message data);
Message unpack(char* data);


#endif //CMESS_PACKETS_HPP