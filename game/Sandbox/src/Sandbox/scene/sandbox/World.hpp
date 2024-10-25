#pragma once
#include "Syris/materials/MaterialManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Sandbox/shaders/sandbox_scene_layout.h"
#include "GridLookUp.hpp"

namespace Sandbox{
    class World{
        public:
        struct CreateInfo{
            Syris::MaterialManager& m_material_manager;
            Syris::ShaderManager& m_shader_manager;
            Syris::EntityManager& m_entity_manager;
            glm::uvec2 world_dimmensions;
            Syris::Statistics& statistics;
        };
        World(CreateInfo info);
        ~World();
        World(const World& ref) = delete;

        void draw(sandbox_scene::ShaderLayoutTuple data);
        //void init();
        //void shutdown();
        private:
            bool m_is_init = false;
            Syris::EntityManager& m_entity_manager;
            Syris::MaterialManager& m_material_manager;
            Syris::ShaderManager& m_shader_manager;  
            Syris::MaterialManager::MaterialID m_tile_material;
            glm::uvec2 m_world_dimmensions;
            GridLookUp* m_world_tiles;
            Syris::ShaderManager::ShaderID m_shader_id;
    };
}