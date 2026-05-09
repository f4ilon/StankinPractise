cmake_minimum_required(VERSION 3.14)
project(StankinPractise LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)

# === СЕРВЕР ===
add_executable(Server
    Server/Server_main.cpp
    Server/Server.cpp
    Common/Packets.cpp     # <-- Добавили это
)

find_package(Boost REQUIRED)
target_link_libraries(Server Boost::boost pthread)
target_compile_definitions(Server PRIVATE LINUX_BUILD)

# === КЛИЕНТ ===
add_executable(Client
    Client/Client_main.cpp
    Client/Client.cpp
    Client/Utils.cpp
    Common/Packets.cpp
)

target_link_libraries(Client pthread)
target_compile_definitions(Client PRIVATE LINUX_BUILD)
