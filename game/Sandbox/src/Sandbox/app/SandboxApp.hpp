#pragma once
#include <entt.hpp>

#include "Syris/app/Application.hpp"
#include "Syris/utils/EngineTime.hpp"
#include "Syris/app/Window.hpp"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/renderer/renderer2D/Renderer.h"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/texture/Texture.hpp"
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/Jobs/ThreadPool.hpp"
#include "Syris/scene/SceneManager.hpp"
#include "Syris/scripts/DLL_Watcher.hpp"
#include "Syris/utils/filesystem/DirWatcher.hpp"

#include "SandboxLayers.hpp"
#include "Syris/utils/containers/Singleton.hpp"

//data application
//layers that use that data
//async, sending data, etc
//where to store the data

namespace Sandbox{

    class SandboxApp : public Syris::Application, Syris::Layer{
    public:
        struct CreateInfo
        {
            const char *atlas_path;
            Syris::GraphicsContext::CreateInfo gc_info;
            Syris::OrthographicCameraController::CreateInfo camera_info;
            Syris::AppInit app_init;
        };
        SandboxApp(CreateInfo &info);
        ~SandboxApp();
        void run();
        void on_update(const Syris::engine_time::Time &time) override;
        bool on_event(Syris::Event *event) override;

    private:
        void set_scene(Syris::SceneID scene);
        std::unique_ptr<Syris::GraphicsContext> m_graphics_context;
        //SandboxData m_data;
        //Syris::engine_time::Time m_time;

        Syris::SceneManager m_scene_manager;

        Syris::SceneID m_scene_simple;
        Syris::SceneID m_scene_triangle;
        Syris::SceneID m_scene_sandbox;

        Syris::SceneID m_current_scene_id;
        Syris::SceneID m_next_scene;

        Syris::LayerID m_current_layer_id;

        Syris::Statistics &m_statistics;
        //ImGuiLayer m_gui_layer;
        //SandboxLayer *m_layer; // ok, pass &data handle at construction
        Syris::engine_time::Time m_time;
        Syris::ThreadPool m_thread_pool{6};
        Syris::DLL_Watcher m_dll_scripts;
        Syris::DirWatcher
    }; 
}
