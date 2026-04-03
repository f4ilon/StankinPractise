#include "Utils.h"


std::string WCharToString(wchar_t wch) {
    char buf[5] = {};
    WideCharToMultiByte(CP_UTF8, 0, &wch, 1, buf, sizeof(buf), nullptr, nullptr);
    return std::string(buf);
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
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


int utf8_visible_length(const std::string& str) {
    int length = 0;
    for (char c : str) {
        // Если старшие два бита не равны 10 (0x80), значит это начало нового символа
        if ((c & 0xC0) != 0x80) {
            length++;
        }
    }
    return length;
}