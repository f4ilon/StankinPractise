cmake_minimum_required(VERSION 3.20) # Исправлена версия (4.x еще не существует)

# Убраны скобки < >. Имя проекта просто Cmess
project(Cmess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Сохраняем путь до директории include (если у вас хедеры и там, и там, лучше добавить оба)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)


# --- ЦЕЛИ СБОРКИ ---
# Серверная часть
add_executable(Server Server/Server_main.cpp Server/Server.cpp Server/ThreadPool.cpp)
# Клиентская часть
add_executable(Client Client/Client_main.cpp Client/Client.cpp)