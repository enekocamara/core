#pragma once
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/texture/FrameTexture2D.hpp"
#include "GridLookUp.hpp"
#include "Sandbox/shaders/fragment_tile_rendering.h"

namespace Sandbox{
    class World{
        public:
        struct CreateInfo{
            Syris::BatchRendererManager& m_material_manager;
            Syris::ShaderManager& m_shader_manager;
            Syris::EntityManager& m_entity_manager;
            glm::uvec2 world_dimmensions;
            Syris::Statistics& statistics;
            int seed;
        };
        World(CreateInfo info);
        ~World();
        World(const World& ref) = delete;
        void gen_2d_noise_vector(std::vector<float>& texture, glm::uvec2 dimmensions, int seed);
        void draw(Syris::Uniform *uniforms);
        void generate_ground();
        entt::entity gen_ground_tile(glm::uvec2 pos, Syris::EntityManager& entity_manager, uint32_t index);

        
        private:
            uint32_t idx(glm::uvec2 pos){return pos.y * m_world_dimmensions.x + pos.x;}
            void make_tile_renderer();
            int m_seed;
            Syris::EntityManager& m_entity_manager;
            Syris::BatchRendererManager& m_renderer_manager;
            Syris::ShaderManager& m_shader_manager;  
            Syris::BatchRendererManager::BR_ID m_tile_renderer_id;
            glm::uvec2 m_world_dimmensions;
            GridLookUp* m_world_tiles;

            std::vector<float> m_temperature;
            std::vector<float> m_humidity;
            std::vector<float> m_height;

            std::unique_ptr<Syris::Texture2D> m_temp_text;
            std::unique_ptr<Syris::Texture2D> m_humd_text;
            std::unique_ptr<Syris::Texture2D> m_height_text;

            std::unique_ptr<Syris::FrameTexture2D> m_temp_frame_text;
            Syris::ShaderManager::ShaderID m_texture_shader;

            Syris::ShaderManager::ShaderID m_shader_id;
            Syris::Statistics& m_statistics;
    };
}