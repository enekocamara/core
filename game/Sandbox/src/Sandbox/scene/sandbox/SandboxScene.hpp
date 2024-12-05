#pragma once
#include <entt.hpp>

#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/Jobs/ThreadPool.hpp"
#include "Syris/scripts/DynamicLibraryLoader.hpp"

#include "Sandbox/ecs/CollectableManager.hpp"
#include "Sandbox/world_generator/World.hpp"
#include "AsyncToSyncQueue.hpp"

namespace Sandbox{
    namespace config{
        static constexpr size_t tile_size = 16;
        static constexpr size_t render_tile_size = 64;
        struct MapConfig{
            size_t num_tiles_x;
            size_t num_tiles_y;
        };
    }

    class SandboxScene : public Syris::Scene{
        public:
            struct CreateInfo
            {
                // entt::registry &registry;
                const char *atlas_path;
                Syris::ShaderManager &shader_manager;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics;
                Syris::ThreadPool& thread_pool;
                Syris::GraphicsContext& graphics_context;
                Syris::DynamicLibraryLoader& dll;
            };
            SandboxScene(CreateInfo info);
            ~SandboxScene();

            void on_update(const Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
            void sim_loop();
            Syris::StatisticModID get_statistic_mod_ID(){return m_statistic_mod_ID;}
        private:

            void update_data(bool imgui);
            void make_entity_renderer();
            //entt::registry& m_registry;
            Syris::BatchRendererManager m_material_manager;
            Syris::EntityManager m_entity_manager;
            Syris::TextureAtlas m_texture_atlas;
           // Syris::RenderBuffer* m_buffer;
            Syris::ShaderManager& m_shader_manager;
            Syris::ThreadPool& m_thread_pool;
            Syris::GraphicsContext& m_graphics_context;
            Syris::DynamicLibraryLoader& m_dll;


            glm::uvec2 m_render_window_size = {0,0};
            Syris::OrthographicCameraController m_camera;
            //Syris::BatchRendererManager::ER_IDle_material;
            Syris::BatchRendererManager::BR_ID m_entity_renderer_id;
            entt::entity m_player_id;
            uint32_t m_shader_id;
            World *m_world;

            /*Statistics */
            Syris::Statistics& m_statistics;
            Syris::StatisticModID m_statistic_mod_ID;
            //GridLookUp m_world_tiles;


            //async
            AsyncToSyncQueue m_async_to_sync_queue;

            //sim
            uint32_t m_sim_thread_count = 4;
            std::atomic_bool m_sim_loop_running = true;
            //Syris::engine_time::Time m_sim_time;
            Syris::engine_time::FPS m_sim_fps;
            //std::thread m_sim_thread;

    };
}
