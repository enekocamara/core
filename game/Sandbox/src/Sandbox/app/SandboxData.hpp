#pragma once
#include <entt.hpp>

#include "Syris/context/GraphicsContext.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"

#include "Sandbox/scene/sandbox/SandboxScene.hpp"
#include "Sandbox/scene/TriangleScene.hpp"
#include "Sandbox/scene/SimpleScene.hpp"
#include "Sandbox/layers/ImGuiLayer.hpp"


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
            };
            SandboxData(CreateInfo info);
            ~SandboxData();
            [[nodiscard]] entt::registry& get_registry() {return m_registry;}

            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event) override;

            //[[nodiscard]] SandboxScene& get_scene() {return *m_scene;}
        private:

            void set_scene(Scenes scene);

            entt::registry& m_registry;

            SimpleScene* m_scene_simple;
            TriangleScene* m_scene_triangle;
            SandboxScene* m_scene_sandbox;
            
            Syris::LayerID m_current_scene_id;

            Scenes m_current_scene = Scenes::Triangle;
            Scenes m_next_scene = Scenes::Triangle;
            //Syris::Layer m_gui_layer;
            Syris::GraphicsContext& m_graphics_context;
            ImGuiLayer m_gui_layer;
            SandboxLayer* m_layer;//ok, pass &data handle at construction
    };
}
#include "SandboxLayers.hpp"