#include <gtest/gtest.h>

// Подключаем файлы из других папок
#include "../Client/Utils.h"
#include "../Common/Packets.h"


// ТЕСТЫ ДЛЯ УТИЛИТ (Client/Utils.cpp)

TEST(UtilsTests, Utf8VisibleLengthWorks) {
   EXPECT_EQ(utf8_visible_length("Hello"), 5);
   EXPECT_EQ(utf8_visible_length("Привет"), 6); // Русские символы
   EXPECT_EQ(utf8_visible_length(""), 0);
   EXPECT_EQ(utf8_visible_length("Hello Привет"), 12);
}

TEST(UtilsTests, WStringToStringConversion) {
   std::wstring wide_str = L"TestString";
   EXPECT_EQ(WStringToString(wide_str), "TestString");
   EXPECT_EQ(WStringToString(L""), "");
}

TEST(UtilsTests, StringToWStringConversion) {
   std::string normal_str = "C++";
   EXPECT_EQ(StringToWString(normal_str), L"C++");
   EXPECT_EQ(StringToWString(""), L"");
}

TEST(UtilsTests, SingleWCharToString) {
   wchar_t letter = L'A';
   EXPECT_EQ(WCharToString(letter), "A");
}

// ТЕСТЫ ДЛЯ ПАКЕТОВ (Common/Packets.cpp)

// Тестируем функцию pack() - упаковка структуры в строку
TEST(PacketsTests, PackMessageWorks) {
   Message msg;
   msg.type = "standardMessage";
   msg.fromUser = "Ivan";
   msg.message = "Hello World";

   // Согласно Packets.cpp, формат должен быть: type~fromUser~message~
   std::string expected_str = "standardMessage~Ivan~Hello World~";

   EXPECT_EQ(pack(msg), expected_str);
}

// Тестируем функцию unpack() - распаковка строки обратно в структуру
TEST(PacketsTests, UnpackMessageWorks) {
   std::string raw_data = "changeRoom~Server~General~";

   Message decoded_msg = unpack(raw_data);

   EXPECT_EQ(decoded_msg.type, "changeRoom");
   EXPECT_EQ(decoded_msg.fromUser, "Server");
   EXPECT_EQ(decoded_msg.message, "General");
}

// Тест на симметричность - если упаковать, а потом распаковать, данные не должны измениться
TEST(PacketsTests, PackAndUnpackSymmetry) {
   Message original_msg;
   original_msg.type = "testType";
   original_msg.fromUser = "TestUser";
   original_msg.message = "Test message with spaces!";

   // Упаковываем
   std::string packed_string = pack(original_msg);
   // Сразу распаковываем
   Message result_msg = unpack(packed_string);

   // Сверяем поля
   EXPECT_EQ(result_msg.type, original_msg.type);
   EXPECT_EQ(result_msg.fromUser, original_msg.fromUser);
   EXPECT_EQ(result_msg.message, original_msg.message);
}

// Тестируем поведение при пустых полях
TEST(PacketsTests, EmptyFieldsHandling) {
   Message empty_msg;
   empty_msg.type = "";
   empty_msg.fromUser = "";
   empty_msg.message = "";

   std::string packed = pack(empty_msg);
   EXPECT_EQ(packed, "~~~"); // Должны быть просто разделители

   Message unpacked = unpack(packed);
   EXPECT_EQ(unpacked.type, "");
   EXPECT_EQ(unpacked.fromUser, "");
   EXPECT_EQ(unpacked.message, "");
}

// Тест на известную уязвимость протокола: символ-разделитель внутри сообщения
TEST(PacketsTests, DelimiterBreaksMessageWarning) {
   Message msg;
   msg.type = "msg";
   msg.fromUser = "User";
   msg.message = "Hello~world"; // Юзер ввел тильду

   std::string packed = pack(msg);
   Message unpacked = unpack(packed);

   // Сообщение обрезается из-за логики парсера. Тест фиксирует эту особенность.
   EXPECT_NE(unpacked.message, msg.message); 
   EXPECT_EQ(unpacked.message, "Hello"); 
}

// Тест на обработку экстремально длинного сообщения
TEST(PacketsTests, HugeMessageHandling) {
   Message msg;
   msg.type = "standardMessage";
   msg.fromUser = "Spammer";
   msg.message = std::string(10000, 'A'); // Строка из 10 000 символов

   std::string packed = pack(msg);
   Message unpacked = unpack(packed);

   EXPECT_EQ(unpacked.message.length(), 10000);
   EXPECT_EQ(unpacked.message, msg.message);
}

// Проверка логики очистки пробелов
TEST(UtilsTests, TrimSpacesLogic) {
   std::string input = "General   \n\r\t";
   input.erase(input.find_last_not_of(" \n\r\t") + 1);
   
   EXPECT_EQ(input, "General");
}


// ГЛАВНАЯ ФУНКЦИЯ (Точка входа для тестов)
int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}