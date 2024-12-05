#pragma once
#include <iostream>
#include "Syris/log/Log.h"

#ifdef _DEBUG
#define BREAK_POINT(msg) \
    do { \
        CORE_ERROR(std::format("Debug Break: {}", msg)); \
        __debugbreak(); \
    } while (false)
#else
#define BREAK_POINT(msg) \
    do { \
        CORE_ERROR(std::format("BreakPoint: {}", msg)); \
        exit(69); \
    } while (false)
#endif

/*

#ifdef  _DEBUG
#define BREAK_POINT(msg)\
        {CORE_ERROR(std::format("Debug Break: {}",  msg)); \
        __debugbreak();}
#else
#define BREAK_POINT(msg) \
    {CORE_ERROR(std::format("BreakPoint: {}",  msg)); \
    exit(69);}
#endif
*/