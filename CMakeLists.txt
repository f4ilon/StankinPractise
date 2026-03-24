cmake_minimum_required(VERSION 3.20)

project(Cmess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)
include_directories("C:/msys64/ucrt64/include")
# Если ставили в mingw64, раскомментируйте строку ниже вместо верхней:
#include_directories("C:/msys64/mingw64/include")


# --- ЦЕЛИ СБОРКИ ---
# Серверная часть
add_executable(Server
        Server/Server_main.cpp
        Server/Server.cpp
)
target_link_libraries(Server ws2_32 wsock32)

# Клиентская часть
add_executable(Client Common/Packets.h Client/Client_main.cpp Client/Client.cpp
        Common/Packets.cpp)
