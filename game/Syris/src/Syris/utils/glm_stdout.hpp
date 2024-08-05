#pragma once
#include <iostream>
#include <glm/glm.hpp>

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

