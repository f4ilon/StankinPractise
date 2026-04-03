#include "Server.h"
#include <iostream>


using boost::asio::ip::tcp;


Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Передаем ссылку на сервер (*this) в сессию
                std::make_shared<Session>(std::move(socket), *this)->start();
            }
            do_accept();
        });
}

// Добавить клиента в комнату
void Server::join_room(std::shared_ptr<Session> session, const std::string& room_name) {
    rooms_[room_name].insert(session);
    std::cout << "Клиент вошел в комнату: " << room_name << " (Людей: " << rooms_[room_name].size() << ")\n";
}

// Удалить клиента из комнаты
void Server::leave_room(std::shared_ptr<Session> session, const std::string& room_name) {
    if (rooms_.count(room_name)) {
        rooms_[room_name].erase(session);
        // Если комната опустела, удаляем её из памяти
        if (rooms_[room_name].empty()) {
            rooms_.erase(room_name);
        }
    }
}

// Разослать сообщение ТОЛЬКО участникам конкретной комнаты
void Server::broadcast_to_room(const std::string& room_name, const std::string& msg, std::shared_ptr<Session> sender) {
    if (rooms_.count(room_name)) {
        for (auto& participant : rooms_[room_name]) {
            if (participant != sender) { // Себе не отправляем
                participant->deliver(msg);
            }
        }
    }
}


Session::Session(tcp::socket socket, Server& server)
    : socket_(std::move(socket)), server_(server), current_room_("General") {} // По умолчанию кидаем в General

void Session::start() {
    server_.join_room(shared_from_this(), current_room_);
    do_read();
}

void Session::deliver(const std::string& msg) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(msg),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                server_.leave_room(self, current_room_);
            }
        });
}

// Обработка текста перед рассылкой (Парсер команд)
void Session::process_message(const std::string& raw_msg) {
    Message message = unpack(raw_msg);
    std::string command = message.message;

    if (command[0] != '/') {
        // Если это обычное сообщение, рассылаем его по ТЕКУЩЕЙ комнате
        server_.broadcast_to_room(current_room_, raw_msg, shared_from_this());
    }
    else {
        // "/join"
        if (command.rfind("/join ", 0) == 0) {
            // Вытаскиваем название новой комнаты (всё, что после пробела)
            std::string new_room = command.substr(6);

            // Убираем невидимые символы (если клиент нажал Enter)
            new_room.erase(new_room.find_last_not_of(" \n\r\t") + 1);

            server_.leave_room(shared_from_this(), current_room_);
            current_room_ = new_room;
            server_.join_room(shared_from_this(), current_room_);

            // Отправляем клиенту системное уведомление
            //deliver("SERVER~Система~Вы успешно перешли в комнату [" + current_room_ + "]~\n");
            Message message;
            message.type = "changeRoom";
            message.fromUser = "Server";
            message.message = current_room_;

            deliver(pack(message));
        }
        // "/leave"
        else if (command.rfind("/leave", 0) == 0) {
            server_.leave_room(shared_from_this(), current_room_);
            current_room_ = "General";
            server_.join_room(shared_from_this(), current_room_);// Кидаем в General
        }
    }
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, 1024),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string received_msg(data_, length);

                process_message(received_msg);

                do_read();
            }
            else {
                std::cout << "Клиент отключился.\n";
                server_.leave_room(self, current_room_); // Удаляем из комнаты при выходе
            }
        });
}