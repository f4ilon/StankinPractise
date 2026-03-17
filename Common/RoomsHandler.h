#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

using SOCKET = int;

struct Room {
   std::string adminName;
   std::unordered_map<std::string, SOCKET> users;
};

class RoomsHandler {
private:
   std::unordered_map<std::string, Room> m_rooms;
   std::string generateRoomKey();

public:
   RoomsHandler() = default;
   ~RoomsHandler() = default;

   std::string createRoom(const std::string& adminName, SOCKET adminSocket);
   bool addUserToRoom(const std::string& roomKey, const std::string& userName, SOCKET socket);
   bool removeUserFromRoom(const std::string& roomKey, const std::string& userName);
   bool removeRoom(const std::string& roomKey, const std::string& requesterName);

   void printAllRooms() const;
   void printUsersInRoom(const std::string& roomKey) const;

   std::string findRoomBySocket(SOCKET socket) const;
   std::string findRoomByName(const std::string& userName) const;
};