#pragma once
#include <iostream>
#include "raylib.h"
#include "glm/glm.hpp"
#include <chrono>
#include <span>
#define MAX_NUMBER_KEYS 249


inline std::ostream& operator<<(std::ostream &os, const glm::vec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}
inline std::ostream& operator<<(std::ostream &os, const glm::ivec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}
namespace config{
    static constexpr size_t tile_size = 16;
    static constexpr size_t render_tile_size = 64;
}
