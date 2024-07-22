#pragma once
#include <iostream>
#include <glad/glad.h>

#include "glm/glm.hpp"
#include <chrono>
#include <span>
#include <entt.hpp>
#define MAX_NUMBER_KEYS 249


inline std::ostream& operator<<(std::ostream &os, const glm::vec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}
inline std::ostream& operator<<(std::ostream &os, const glm::vec4 &vec) {
    os << vec.x << ',' << vec.y << ','<< vec.z << ',' << vec.w; 
    return os;
}
inline std::ostream& operator<<(std::ostream &os, const glm::ivec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}

namespace config{
    static constexpr size_t tile_size = 16;
    static constexpr size_t render_tile_size = 64;
    struct MapConfig{
        size_t num_tiles_x;
        size_t num_tiles_y;
    };

}

#define CHECK_GL_ERROR() \
    { \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) \
        { \
            std::cerr << "OpenGL Error: " << err << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        } \
    }
