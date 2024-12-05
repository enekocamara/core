#pragma once
#include "Syris/utils/Breakpoint.h"

#define CHECK_GL_ERROR() \
    do { \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) \
        { \
            BREAK_POINT(std::format("OpenGL Error: {}", err));\
        } \
    } while(false)
