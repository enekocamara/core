#pragma once
#include "../Libs.h"
namespace scene{
    class Scene{
        public:
            Scene(entt::registry& registry);
            ~Scene();
        private:
            entt::registry& m_registry;
            static constexpr config::MapConfig m_map_config = config::MapConfig{
                .num_tiles_x = 100,
                .num_tiles_y = 100
            };
    };
}
