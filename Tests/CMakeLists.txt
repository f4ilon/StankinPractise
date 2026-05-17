# Ищем Google Test в системе
find_package(GTest REQUIRED)

# Создаем исполняемый файл для тестов
# Передаем сам тест и файлы мессенджера, которые он проверяет
add_executable(messenger_tests 
    test_main.cpp
    ../Common/Packets.cpp
    ../Client/Utils.cpp
)

# Указываем путь к заголовочным файлам (корень проекта)
target_include_directories(messenger_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Линкуем библиотеки GTest
target_link_libraries(messenger_tests GTest::gtest_main)

# Включаем тесты в общую систему CTest
include(CTest)
add_test(NAME AllTests COMMAND messenger_tests)