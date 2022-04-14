#pragma once

#include "iostream"

template<typename ...Args>
inline void dbgln(Args && ...args)
{
    (std::clog << ... << args);
    (std::clog << "\n");
}
