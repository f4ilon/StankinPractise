#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <set>
#include <string>
#include <deque>
#include "../Common/Packets.h"

class Session;
class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

    void join_room(std::shared_ptr<Session> session, const std::string& room_name);
    void leave_room(std::shared_ptr<Session> session, const std::string& room_name);
    void broadcast_to_room(const std::string& room_name, const std::string& msg,
        std::shared_ptr<Session> sender);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::set<std::shared_ptr<Session>>> rooms_;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, Server& server);
    void start();
    void deliver(const std::string& msg);
    void process_message(const std::string& msg);

private:
    void do_read();
    void do_write();

    boost::asio::ip::tcp::socket socket_;
    Server& server_;
    std::string current_room_;
    char data_[1024];
    std::deque<std::string> write_msgs_;
};