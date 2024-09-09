#pragma once
#include <entt.hpp>
#include "../scene/SandboxScene.hpp"
#include "../scene/TriangleScene.hpp"
#include "../scene/SimpleScene.hpp"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
namespace Sandbox{
    //holds the data to the whole application
    //especially the entt owning groups
    class SandboxData{
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
            //[[nodiscard]] SandboxScene& get_scene() {return *m_scene;}
        private:
            entt::registry& m_registry;
            SimpleScene* m_scene;
            Syris::GraphicsContext& m_graphics_context;
    };
}