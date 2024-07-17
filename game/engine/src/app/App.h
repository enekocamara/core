#pragma once
#include "Window.h"
#include "../renderer/renderer2D/Renderer.h"
namespace app{
    struct AppBundle{
        const char* vertex_shader_path;
        const char* fragment_shader_path;
        Window::WindowBundle window_bundle;
        entt::registry& registry;
    };
    class App{
        public:
            App(AppBundle& bundle);
            void run(); 

        private:
            Window m_window;
            scene::Scene m_scene;
            ge::Renderer2D m_renderer;
            entt::registry& m_registry;
            int m_glId;
    };
}
