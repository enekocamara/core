#pragma once
#include <entt.hpp>
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"

namespace Sandbox{
    namespace config{
        static constexpr size_t tile_size = 16;
        static constexpr size_t render_tile_size = 64;
        struct MapConfig{
            size_t num_tiles_x;
            size_t num_tiles_y;
        };

    }

    class SandboxScene : public scene::Scene{
        public:
            SandboxScene(entt::registry& registry, const char* atlas_path);
            ~SandboxScene();


            void onUpdate(GLuint program, glm::mat4 view_projection);
            static constexpr config::MapConfig m_map_config = config::MapConfig{
                .num_tiles_x = 100,
                    .num_tiles_y = 100
            };
        private:
            entt::registry& m_registry;
            Syris::texture::TextureAtlas m_texture_atlas;
            Syris::renderer::RenderBuffer* m_buffer;
    };
}
