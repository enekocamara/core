#pragma once 
#include <unordered_map>
#include "Syris/renderer/Texture.h"

namespace Sandbox::ecs{
    struct Stages{
        Syris::texture::Rectangle2D texture;
        float threashold;
        float comsuption_ms;
    };

    class CollectableManager{
        public:
            using ID = uint32_t;
            const std::vector<Stages>& get_collectable(ID id);
            ID set_collectable(std::vector<Stages>&& collectable);
            void remove_collectable(ID id);
        private:
            ID m_current_id = 1;
            std::unordered_map<ID, std::vector<Stages>> m_collectables;
    };
}