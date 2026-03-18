#include "RoomsHandler.h"
#include <cstdlib>

std::string RoomsHandler::generateRoomKey() {
   const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   std::string key = "XXX-XXX";
   for (int i = 0; i < 7; ++i) {
      if (i == 3) continue;
      key[i] = chars[rand() % chars.length()];
   }
   return key;
}

std::string RoomsHandler::createRoom(const std::string& adminName, SOCKET adminSocket) {
   std::string newKey = generateRoomKey();
   Room newRoom;
   newRoom.adminName = adminName;
   newRoom.users[adminName] = adminSocket;
   m_rooms[newKey] = newRoom;
   return newKey;
}

bool RoomsHandler::addUserToRoom(const std::string& roomKey, const std::string& userName, SOCKET socket) {
   if (m_rooms.count(roomKey) > 0) {
      m_rooms[roomKey].users[userName] = socket;
      return true;
   }
   return false;
}

bool RoomsHandler::removeUserFromRoom(const std::string& roomKey, const std::string& userName) {
   if (m_rooms.count(roomKey) > 0) {
      m_rooms[roomKey].users.erase(userName);
      return true;
   }
   return false;
}

bool RoomsHandler::removeRoom(const std::string& roomKey, const std::string& requesterName) {
   if (m_rooms.count(roomKey) > 0) {
      if (m_rooms[roomKey].adminName == requesterName) {
         m_rooms.erase(roomKey);
         return true;
      }
   }
   return false;
}

void RoomsHandler::printAllRooms() const {
   std::cout << "\n--- All Rooms ---\n";
   for (const auto& pair : m_rooms) {
      std::cout << pair.first << " (Admin: " << pair.second.adminName << ")\n";
   }
}

void RoomsHandler::printUsersInRoom(const std::string& roomKey) const {
   if (m_rooms.count(roomKey) > 0) {
      std::cout << "\nUsers in [" << roomKey << "]:\n";
      for (const auto& userPair : m_rooms.at(roomKey).users) {
         std::cout << " - " << userPair.first << " [" << userPair.second << "]\n";
      }
   }
}

std::string RoomsHandler::findRoomBySocket(SOCKET socket) const {
   for (const auto& roomPair : m_rooms) {
      for (const auto& userPair : roomPair.second.users) {
         if (userPair.second == socket) return roomPair.first;
      }
   }
   return "";
}

std::string RoomsHandler::findRoomByName(const std::string& userName) const {
   for (const auto& roomPair : m_rooms) {
      if (roomPair.second.users.count(userName) > 0) return roomPair.first;
   }
   return "";
}