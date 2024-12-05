#pragma once
#include <vector>
#include <entt.hpp>

namespace Syris{
    class Chunk{
        public:
            Chunk() = default;
            std::vector<entt::entity> m_entities;
    };
}