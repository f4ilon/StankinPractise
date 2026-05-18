cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")

project(StankinPractise LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(GTest QUIET)
if(GTest_FOUND)
    add_executable(messenger_tests
        Tests/test_main.cpp
        Common/Packets.cpp
        Client/Utils.cpp
    )

    target_include_directories(messenger_tests PRIVATE ${CMAKE_SOURCE_DIR})

    if(WIN32)
        target_link_libraries(messenger_tests GTest::gtest_main ws2_32)
    else()
        target_link_libraries(messenger_tests GTest::gtest_main pthread)
    endif()

    include(CTest)
    add_test(NAME AllTests COMMAND messenger_tests)
endif()