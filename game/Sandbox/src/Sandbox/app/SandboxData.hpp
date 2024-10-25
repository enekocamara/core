#pragma once
#include <entt.hpp>

#include "Syris/context/GraphicsContext.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/scene/SceneManager.hpp"

#include "Sandbox/scene/sandbox/SandboxScene.hpp"
#include "Sandbox/scene/TriangleScene.hpp"
#include "Sandbox/scene/SimpleScene.hpp"
#include "Sandbox/layers/ImGuiLayer.hpp"
#include "Syris/statistics/Statistics.hpp"

namespace Sandbox{
    //holds the data to the whole application
    //especially the entt owning groups
    enum class Scenes{
        Triangle,
        Simple,
        Sandbox
    };
    class SandboxLayer;
    class SandboxData : public Syris::Layer{
        public:
            struct CreateInfo{
                entt::registry& registry;
                const char * atlas_path;
                Syris::GraphicsContext& graphics_context;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics; 
            };
            SandboxData(CreateInfo info);
            ~SandboxData();
            [[nodiscard]] entt::registry& get_registry() {return m_registry;}

            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event) override;

            //[[nodiscard]] SandboxScene& get_scene() {return *m_scene;}
        private:

            void set_scene(Syris::SceneID scene);

            entt::registry& m_registry;
            Syris::SceneManager m_scene_manager;
            
            Syris::SceneID m_scene_simple;
            Syris::SceneID m_scene_triangle;
            Syris::SceneID m_scene_sandbox;

            Syris::SceneID m_current_scene_id;
            Syris::SceneID m_next_scene;
            Syris::LayerID m_current_layer_id;
            //Syris::Layer m_gui_layer;
            Syris::GraphicsContext& m_graphics_context;
            Syris::Statistics& m_statistics;
            ImGuiLayer m_gui_layer;
            SandboxLayer* m_layer;//ok, pass &data handle at construction
    };
}
#include "SandboxLayers.hpp"