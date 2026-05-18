#pragma once

#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

// === Конвертация кодировок ===
std::string WCharToString(wchar_t wch);
std::string WStringToString(const std::wstring& wstr);
std::wstring StringToWString(const std::string& str);
std::string WCharToUTF8(wchar_t wch);

// === Консольные функции ===
void set_cursor(short x, short y);

// Подсчёт видимых символов в UTF-8 строке (важно для кириллицы и эмодзи)
int utf8_visible_length(const std::string& str);