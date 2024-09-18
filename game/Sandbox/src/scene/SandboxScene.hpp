#pragma once
#include <entt.hpp>
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"

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
                entt::registry &registry;
                const char *atlas_path;
                Syris::GraphicsContext &context;
                Syris::OrthographicCameraController::CreateInfo camera_info;
            };
            SandboxScene(CreateInfo info);
            ~SandboxScene();

            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
            static constexpr config::MapConfig m_map_config = config::MapConfig{
                .num_tiles_x = 100,
                .num_tiles_y = 100
            };
        private:

            void update_data(bool imgui);
            entt::registry& m_registry;
            Syris::texture::TextureAtlas m_texture_atlas;
            Syris::renderer::RenderBuffer* m_buffer;
            Syris::GraphicsContext& m_graphics_context;
            Syris::OrthographicCameraController m_camera;
            uint32_t m_shader_id;
    };
}
