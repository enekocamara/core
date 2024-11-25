#pragma once
#include <entt.hpp>
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/statistics/Statistics.hpp"

namespace Sandbox{
    class SimpleScene : public Syris::Scene, public Syris::Layer{
        public:
            struct CreateInfo
            {
                //entt::registry &registry;
                const char *atlas_path;
                Syris::ShaderManager &shader_manager;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics;
            };
            SimpleScene(CreateInfo info);
            ~SimpleScene();

            void on_update(const Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
            void make_batch_renderer();
            Syris::StatisticModID get_statistics()const{return m_statistic_mod_ID;}
            void render_statistics(entt::entity module, entt::registry& registry){
                ImGui::Text("Simple scene info.");
                m_batch_renderer_manager.render(m_batch_renderer_manager.get_statistics(), m_statistics.get_registry());
            };
        private:
            Syris::StatisticModID m_statistic_mod_ID;
            Syris::BatchRendererManager::BR_ID m_batch_renderer_id;
            Syris::ShaderManager& m_shader_manager;
            Syris::BatchRendererManager m_batch_renderer_manager;
            Syris::EntityManager m_entity_manager;
            Syris::Statistics& m_statistics;
            Syris::OrthographicCameraController m_camera;
            uint32_t m_shader_id;
    };
}
