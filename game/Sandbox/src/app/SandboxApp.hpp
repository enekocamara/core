#pragma once
#include <entt.hpp>
#include "Syris/app/OpenGLWindow.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/renderer/renderer2D/Renderer.h"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/renderer/Texture.h"
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/renderAPI/renderApi.h"
#include <Core/Core.hpp>
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "SandboxData.hpp"
#include "Syris/events/Event.hpp"

#include "SandboxLayers.hpp"

//data application
//layers that use that data
//async, sending data, etc
//where to store the data

namespace Sandbox{

    class SandboxApp : public Application{
        public:
            struct CreateInfo{
                const char* atlas_path;
                //Syris::Window::WindowBundle window_bundle;
                Syris::GraphicsContext::CreateInfo gc_info;
                Syris::OrthographicCameraController::CreateInfo camera_info;
            };

            SandboxApp(CreateInfo& info);
            ~SandboxApp();
            void run();
        private:
           // void draw_frame();
            template<typename Group>
                void draw_group(Group& group);

            entt::registry m_registry;
            Syris::GraphicsContext m_graphics_context;
            SandboxData m_data;//holds the data off the whole application  
           // Syris::OpenGLWindow& m_window;
           // scene::Scene* m_scene;
           // Syris::renderer::Renderer2D m_renderer;
           // int m_program;
           // Syris::renderAPI::Quad2D* quad = nullptr;
            Syris::engine_time::Time m_time;
    }; 
}
Application* get_client_app();
