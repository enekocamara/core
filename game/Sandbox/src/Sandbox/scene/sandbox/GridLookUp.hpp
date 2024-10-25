#pragma once

#include <vector>
#include <functional>

#include <entt.hpp>
#include <glm/glm.hpp>

#include "Syris/ecs/EntityManager.hpp"

namespace Sandbox{

    class GridLookUp{
        public:
            struct CreateInfo{
                glm::uvec2 grid_dimmension;
                glm::vec2 tile_dimmensions;
                //glm::vec2 offset? center of the actual grid in world coordinates
                
                std::function<entt::entity(glm::uvec2, Syris::EntityManager&, uint32_t index)> tile_gen;
                Syris::EntityManager& entity_manager;
            };
            GridLookUp(CreateInfo& info);
            ~GridLookUp();
            GridLookUp() = delete;
            GridLookUp(const GridLookUp& ref) = delete;
            
            entt::entity get_entity(uint32_t index);
            entt::entity get_entity(glm::vec2 pos);
        private:
            std::vector<entt::entity> m_entities_grid;
            glm::uvec2 m_grid_dimmensions;
            glm::vec2 m_tile_dimmensions;
    };
}