#pragma once
#include "scene/Scene.hpp"
#include "Libs.h"

class SandboxScene : public scene::Scene{
    public:
        SandboxScene(entt::registry& registry);
        ~SandboxScene();

        static constexpr config::MapConfig m_map_config = config::MapConfig{
            .num_tiles_x = 100,
            .num_tiles_y = 100
        };
    private:
        entt::registry& m_registry;
};
