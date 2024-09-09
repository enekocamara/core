#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Syris{

    struct WindowCreateInfo
    {
        glm::ivec2 dimmensions;
        std::string name;
        bool vSync;
    };

}