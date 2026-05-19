cmake_minimum_required(VERSION 3.14)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


project(StankinPractise LANGUAGES CXX)



include(FetchContent)

# Автоматическая загрузка Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT MSVC) # Проверка переполнений на линукс
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)

# ====================== СЕРВЕР ======================
add_executable(Server
    Server/Server_main.cpp
    Server/Server.cpp
    Common/Packets.cpp
)

find_package(Boost REQUIRED)
target_link_libraries(Server Boost::boost)

if(NOT WIN32)
    target_link_libraries(Server pthread)
endif()

# ====================== КЛИЕНТ ======================
add_executable(Client
    Client/Client_main.cpp
    Client/Client.cpp
    Client/Utils.cpp
    Common/Packets.cpp
)

if(WIN32)
    target_link_libraries(Client ws2_32 wsock32)
else()
    target_link_libraries(Client pthread)
endif()

# ====================== ТЕСТЫ ======================
add_subdirectory(Tests)
# ===================================================
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)