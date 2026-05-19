cmake_minimum_required(VERSION 3.14)



project(MessengerTests LANGUAGES CXX)

# Автоматически скачиваем Google Test из интернета
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Подтягиваем Boost (он уже найден в главном файле, но тут мы его линкуем)
find_package(Boost REQUIRED)

# Создаем исполняемый файл тестов и указываем пути ко всем нужным исходникам
add_executable(messenger_tests 
    test_main.cpp
    ../Common/Packets.cpp
    ../Client/Utils.cpp
    ../Client/Client.cpp
    ../Server/Server.cpp
)

# Объясняем компилятору, где искать заголовочные файлы (.h)
target_include_directories(messenger_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../Server
    ${CMAKE_CURRENT_SOURCE_DIR}/../Client
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
)

# Связываем тесты с Google Test и Boost
target_link_libraries(messenger_tests 
    GTest::gtest_main 
    Boost::boost
)

# Если сборка идет на Windows — добавляем библиотеки сокетов
# Если сборка идет на Windows — добавляем библиотеки сокетов, иначе — pthread
if(WIN32)
    target_link_libraries(messenger_tests ws2_32 wsock32)
else()
    target_link_libraries(messenger_tests pthread)
endif()

# Регистрируем тест в системе CTest
include(CTest)
add_test(NAME AllTests COMMAND messenger_tests)