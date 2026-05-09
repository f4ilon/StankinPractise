#pragma once

#include <string>

// Пустые заглушки для Linux
std::string WCharToString(char wch);
std::string WStringToString(const std::string& wstr);
std::wstring StringToWString(const std::string& str);
void set_cursor(short x, short y);
int utf8_visible_length(const std::string& str);
