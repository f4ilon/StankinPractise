#include "Utils.h"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
#endif

// ====================== WINDOWS ======================
#ifdef _WIN32

std::string WCharToString(wchar_t wch) {
    char buf[5] = {};
    WideCharToMultiByte(CP_UTF8, 0, &wch, 1, buf, sizeof(buf), nullptr, nullptr);
    return std::string(buf);
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WCharToUTF8(wchar_t wch) {
    if (wch == 0) return "";
    char buf[5] = {0};
    WideCharToMultiByte(CP_UTF8, 0, &wch, 1, buf, 4, nullptr, nullptr);
    return std::string(buf);
}

void set_cursor(short x, short y) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x, y});
}

#else

// ====================== LINUX ======================

std::string WCharToString(wchar_t wch) {
    return std::string(1, static_cast<char>(wch)); // упрощённо
}

std::string WStringToString(const std::wstring& wstr) {
    std::string str;
    str.reserve(wstr.length());
    for (wchar_t wc : wstr) {
        str += static_cast<char>(wc); // упрощённо
    }
    return str;
}

std::wstring StringToWString(const std::string& str) {
    std::wstring wstr;
    wstr.reserve(str.length());
    for (char c : str) {
        wstr += static_cast<wchar_t>(c);
    }
    return wstr;
}

std::string WCharToUTF8(wchar_t wch) {
    return std::string(1, static_cast<char>(wch));
}

void set_cursor(short x, short y) {
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H" << std::flush;
}

#endif

// Общая функция (работает на обеих ОС)
int utf8_visible_length(const std::string& str) {
    int length = 0;
    for (unsigned char c : str) {
        if ((c & 0xC0) != 0x80) {  // не continuation byte
            length++;
        }
    }
    return length;
}