#pragma once
#include "Window.h"
#include "renderer/renderer2D/Renderer.h"
#include "renderer/Texture.h"
#include "scene/Scene.hpp"
namespace app{
    struct AppBundle{
        const char* vertex_shader_path;
        const char* fragment_shader_path;
        Window::WindowBundle window_bundle;
        entt::registry& registry;
    };
    class App{
        public:
            App(AppBundle& bundle, scene::Scene* scene);
            void run(); 

        private:
            Window m_window;
            ecs::TextureAtlas m_texture_atlas;
            scene::Scene* m_scene;
            ge::Renderer2D m_renderer;
            entt::registry& m_registry;
            int m_glId;
    };
}
