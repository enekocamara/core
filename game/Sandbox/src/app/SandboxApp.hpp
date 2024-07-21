#pragma once
#include <entt.hpp>
#include "app/Window.h"
#include "scene/Scene.hpp"
#include "renderer/renderer2D/Renderer.h"
#include "renderer/Camera.hpp"
#include  "renderer/Texture.h"


namespace Sandbox{
    class SandboxApp{
        public:
            struct CreateInfo{
                const char* vertex_shader_path;
                const char* fragment_shader_path;
                const char* atlas_path;
                Window::WindowBundle window_bundle;
                entt::registry& registry;
                Syris::CameraOrthographic::CreateInfo cameraCreateInfo;
            };

            SandboxApp(CreateInfo info);
            ~SandboxApp();
            void run();
        private:
            void draw_frame();
            template<typename Group>
                void draw_group(Group& group);

            Window m_window;
            Syris::texture::TextureAtlas m_texture_atlas;
            scene::Scene* m_scene;
            Syris::renderer::Renderer2D m_renderer;
            entt::registry& m_registry;
            Syris::CameraOrthographic m_camera;
            int m_program;
            Syris::renderer::Triangle2D triangle;
            Syris::renderer::Quad2D quad;
    };
}
