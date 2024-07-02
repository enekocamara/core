#pragma once
#include <iostream>
#include "raylib.h"
#include "glm/glm.hpp"
#include <chrono>
#define MAX_NUMBER_KEYS 249


inline std::ostream& operator<<(std::ostream &os, const glm::vec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}
inline std::ostream& operator<<(std::ostream &os, const glm::ivec2 &vec) {
    os << vec.x << ',' << vec.y;
    return os;
}
