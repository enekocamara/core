#pragma once
#include <entt.hpp>
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/materials/MaterialManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"

namespace Sandbox{
    class SimpleScene : public Syris::Scene, public Syris::Layer{
        public:
            struct CreateInfo
            {
                //entt::registry &registry;
                const char *atlas_path;
                Syris::GraphicsContext &context;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics;
            };
            SimpleScene(CreateInfo info);
            ~SimpleScene();

            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
        private:
            Syris::MaterialManager::MaterialID m_material;
            Syris::GraphicsContext& m_graphics_context;
            Syris::MaterialManager m_material_manager;
            Syris::EntityManager m_entity_manager;
            Syris::OrthographicCameraController m_camera;
            uint32_t m_shader_id;
    };
}
