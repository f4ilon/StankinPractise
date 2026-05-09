#include "Utils.h"
#include <iostream>
#include <string>

std::string WCharToString(char wch) { 
    return std::string(1, wch); 
}

std::string WStringToString(const std::string& wstr) { 
    return wstr; 
}

std::wstring StringToWString(const std::string& str) { 
    return std::wstring(str.begin(), str.end()); 
}

void set_cursor(short x, short y) {
    std::cout << "\033[" << y+1 << ";" << x+1 << "H" << std::flush;
}

int utf8_visible_length(const std::string& str) {
    int length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) length++;
    }
    return length;
}
