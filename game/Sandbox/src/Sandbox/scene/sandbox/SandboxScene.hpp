#pragma once
#include <entt.hpp>
#include <hpx/thread.hpp>

#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/materials/MaterialManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Sandbox/ecs/CollectableManager.hpp"
#include "World.hpp"
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

    class SandboxScene : public Syris::Scene, public Syris::Layer{
        public:
            struct CreateInfo
            {
                // entt::registry &registry;
                const char *atlas_path;
                Syris::GraphicsContext &context;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics;
            };
            SandboxScene(CreateInfo info);
            ~SandboxScene();

            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
            void sim_loop();
            Syris::StatisticModID get_statistic_mod_ID(){return m_statistic_mod_ID;}
            static constexpr config::MapConfig m_map_config = config::MapConfig{
                .num_tiles_x = 2,
                .num_tiles_y = 2
            };
        private:

            void update_data(bool imgui);
            //entt::registry& m_registry;
            Syris::MaterialManager m_material_manager;
            Syris::EntityManager m_entity_manager;
            Syris::texture::TextureAtlas m_texture_atlas;
           // Syris::RenderBuffer* m_buffer;
            Syris::GraphicsContext& m_graphics_context;
            Syris::OrthographicCameraController m_camera;
            //Syris::MaterialManager::MaterialID m_tile_material;
            Syris::MaterialManager::MaterialID m_entity_material_id;
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
            std::atomic_bool m_sim_loop_running = true;
            //Syris::engine_time::Time m_sim_time;
            Syris::engine_time::FPS m_sim_fps;

    };
}
