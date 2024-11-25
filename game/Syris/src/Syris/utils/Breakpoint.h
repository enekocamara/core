#pragma once
#include <iostream>

#ifdef  _DEBUG
#define BREAK_POINT(msg)\
        {std::cerr << "Debug Break: " << msg << std::endl; \
        __debugbreak();}
#else
#define BREAK_POINT(msg) \
    std::cerr << "Debug Break: " << msg << std::endl; \
    exit(69);
#endif

namespace Syris{
    inline void break_point(const char *message)
    {
        std::cerr << "Debug Break: " << message << std::endl;
#ifdef _DEBUG
        __debugbreak();
#else
        exit(1);
#endif
    }
}