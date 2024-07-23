#pragma once
#include <entt.hpp>
#include "Syris/app/Window.h"
#include "Syris/scene/Scene.hpp"
#include "Syris/renderer/renderer2D/Renderer.h"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include  "Syris/renderer/Texture.h"
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/renderAPI/renderApi.h"

namespace Sandbox{

    struct SimpleVertex{};

    class SandboxApp{
        public:
            struct CreateInfo{
                const char* vertex_shader_path;
                const char* fragment_shader_path;
                const char* atlas_path;
                Syris::Window::WindowBundle window_bundle;
                entt::registry& registry;
                Syris::OrthographicCameraController::CreateInfo cameraCreateInfo;
            };

            SandboxApp(CreateInfo info);
            ~SandboxApp();
            void run();
        private:
            void draw_frame();
            template<typename Group>
                void draw_group(Group& group);

            Syris::Window m_window;
            scene::Scene* m_scene;
            Syris::renderer::Renderer2D m_renderer;
            entt::registry& m_registry;
            Syris::OrthographicCameraController m_camera;
            int m_program;
            Syris::renderAPI::Quad2D* quad = nullptr;
            Syris::engine_time::Time m_time;
    };
}
