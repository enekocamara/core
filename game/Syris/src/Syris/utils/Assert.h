#pragma once
#include "Breakpoint.h"

#define ASSERT(condition, message)\
    do{\
        if (!(condition))\
            BREAK_POINT(message);\
    }while(false)