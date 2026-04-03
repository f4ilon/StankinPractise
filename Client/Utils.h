#pragma once

#include <iostream>
#include <windows.h>
#include <string>


std::string WCharToString(wchar_t wch);
// Из UTF-16 (Windows UI) в UTF-8 (Сеть)
std::string WStringToString(const std::wstring& wstr);
// Из UTF-8 (Сеть) в UTF-16 (Windows UI)
std::wstring StringToWString(const std::string& str);
// Конвертирует широкий символ Windows (UTF-16) в строку UTF-8
std::string WCharToUTF8(wchar_t wch);
// Установка курсора в консоли
void set_cursor(short x, short y);
// Функция для подсчета ВИДИМЫХ символов в строке UTF-8 (а не байтов)
int utf8_visible_length(const std::string& str);