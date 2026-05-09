#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <functional>
#include "../Common/Packets.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define CLOSE_SOCKET close
#endif

class Client {
public:
    socket_t Socket;
    sockaddr_in serverAddr{};
    std::string name;
    std::string room = "General";

    Client();
    bool tryConnect();
    void sendMessage(std::string data);
    std::function<void(const std::string&)> onMessageReceived;
    void getMessage();
    void stop();
};
