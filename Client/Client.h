#pragma once

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "../Common/Packets.h"


class Client {
public:
    SOCKET Socket;
    sockaddr_in serverAddr{};
    std::string name;
    Client();
    void sendMessage(std::string data);
    void getMessage();
};