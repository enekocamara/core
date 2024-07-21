#pragma once
#include "app/Window.h"
#include "scene/Scene.hpp"
#include <entt.hpp>
#include "renderer/renderer2D/Renderer.h"
#include "app/App.h"
#include "renderer/Camera.hpp"

class SandboxApp{
    public:
        struct CreateInfo{
            const char* vertex_shader_path;
            const char* fragment_shader_path;
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
        ecs::TextureAtlas m_texture_atlas;
        scene::Scene* m_scene;
        ge::Renderer2D m_renderer;
        entt::registry& m_registry;
        Syris::CameraOrthographic m_camera;
        int m_program;
        ge::Triangle2D triangle;
        ge::Quad2D quad;
};
