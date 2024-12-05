#pragma once
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/texture/FrameTexture2D.hpp"
#include "Syris/texture/TextureEditor.hpp"
#include "Syris/utils/containers/VectorStorage.hpp"
#include "Syris/utils/noise/NoiseGenerator.hpp"

#include "Syris/utils/containers/GridLookUp.hpp"
#include "Syris/utils/world_generation/ChunkSystem.hpp"

namespace Sandbox{
    class World{
        public:
        struct CreateInfo{
            Syris::BatchRendererManager& m_material_manager;
            Syris::ShaderManager& m_shader_manager;
            Syris::EntityManager& m_entity_manager;
            //glm::uvec2 world_dimmensions;
            glm::uvec2 chunks;
            glm::uvec2 chunk_size;
            glm::vec2 tile_size;
            Syris::Statistics& statistics;
            Syris::BatchRendererManager::BR_ID entity_renderer_id;
            int seed;
        };
        World(CreateInfo info);
        ~World();
        World(const World& ref) = delete;
        void draw(Syris::Uniform *uniforms);
        void create_world();
        void create_textures();
        void update_ground_tiles();

        const Syris::GridLookUp<entt::entity>* get_grid_look_up()const {return m_world_tiles;};
        ///std::optional<Syris::ChunkSystem& get_chunk_system(){return m_chunk_system;}


        private:
            uint32_t idx(glm::uvec2 pos){return pos.y * m_world_dimmensions.x + pos.x;}
            void make_tile_renderer();
            int m_seed;
            Syris::EntityManager& m_entity_manager;
            Syris::BatchRendererManager& m_renderer_manager;
            Syris::ShaderManager& m_shader_manager;  
            Syris::BatchRendererManager::BR_ID m_tile_renderer_id;
            Syris::BatchRendererManager::BR_ID m_entity_renderer_id;
            glm::uvec2 m_world_dimmensions;
            Syris::GridLookUp<entt::entity>* m_world_tiles;
            //Syris::ChunkSystem m_chunk_system;

            std::unique_ptr<Syris::Texture2D> m_combined_text;
            
            std::unique_ptr<Syris::FrameTexture2D> m_combined_frame_text;
            /*
            std::unique_ptr<Syris::FrameTexture2D> m_humd_frame_text;
            std::unique_ptr<Syris::FrameTexture2D> m_height_frame_text;
            */
            Syris::ShaderManager::ShaderID m_texture_shader;

            std::unique_ptr<Syris::TextureEditor> m_combined_texture_editor;

            //Syris::VectorStorage<ecs::CTileData> m_storage;

            Syris::NoisePattern m_noise_pattern;

            Syris::ShaderManager::ShaderID m_shader_id;
            Syris::Statistics& m_statistics;
    };
}