# Ищем Google Test в системе
find_package(GTest REQUIRED)

# Создаем исполняемый файл для тестов
add_executable(messenger_tests 
    test_main.cpp
    ../Common/Packets.cpp
    ../Client/Utils.cpp
)

# Указываем путь к заголовочным файлам
target_include_directories(messenger_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Линкуем библиотеки GTest и сеть Windows
target_link_libraries(messenger_tests GTest::gtest_main)
target_link_libraries(messenger_tests ws2_32)

include(CTest)
add_test(NAME AllTests COMMAND messenger_tests)