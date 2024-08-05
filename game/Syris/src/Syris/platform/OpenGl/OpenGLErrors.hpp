#pragma once
#include "Syris/log/Log.h"

#define CHECK_GL_ERROR() \
    { \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) \
        { \
            CORE_ERROR(std::format("OpenGL Error: {}", err));\
        } \
    }
