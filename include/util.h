#pragma once

#include "iostream"

template<typename ...Args>
inline void dbgln(Args && ...args)
{
    (std::clog << ... << args);
    (std::clog << "\n");
}

inline std::string hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (auto i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
}

inline std::string hex_to_ascii(std::string hex)
{
    std::string ascii = "";
    auto hex_number = stoi(hex, 0, 16);

    if (hex_number >= 0x20 && hex_number < 0x7f) {
        for (size_t i = 0; i < hex.length(); i += 2) {
            auto part = hex.substr(i, 2);
            char ch = stoul(part, nullptr, 16);
            ascii += ch;
        }
        return ascii;
    }
    return ".";
}
