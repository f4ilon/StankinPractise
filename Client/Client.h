#pragma once

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include "../Common/Packets.h"
#include <functional>



class Client {
public:
    SOCKET Socket;
    sockaddr_in serverAddr{};
    std::string name;
    std::string room = "General";
    Client();
    bool tryConnect();
    void sendMessage(std::string data);
    std::function<void(const std::string&)> onMessageReceived; // <--- ДОБАВИТЬ ЭТО
    void getMessage();
    void stop();
};