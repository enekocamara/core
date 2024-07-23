#pragma once
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"

namespace Sandbox{
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
