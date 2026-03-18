cmake_minimum_required(VERSION 3.20)

project(Cmess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)

# --- ЦЕЛИ СБОРКИ ---
# Серверная часть
add_executable(Server Server/Server_main.cpp Server/Server.cpp Server/ThreadPool.cpp)
# Клиентская часть
add_executable(Client Common/Packets.hpp Client/Client_main.cpp Client/Client.cpp)