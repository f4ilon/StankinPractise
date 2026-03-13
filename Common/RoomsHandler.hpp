#ifndef CMESS_ROOMSHANDLER_HPP
#define CMESS_ROOMSHANDLER_HPP
#include <map>
#include <string>
#include <winsock2.h>


class Room {
    char key[7];
    std::map<std::string, SOCKET> users;
};


#endif //CMESS_ROOMSHANDLER_HPP