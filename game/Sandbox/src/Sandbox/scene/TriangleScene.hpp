#pragma once
#include <entt.hpp>
#include "Syris/renderer/RenderBuffer.hpp"
#include "Syris/scene/Scene.hpp"
#include "Syris/Libs.h"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/events/Event.hpp"
#include "Syris/renderer/camera/OrthographicCameraController.hpp"
#include "Syris/statistics/Statistics.hpp"

namespace Sandbox{
    class TriangleScene : public Syris::Scene, public Syris::Layer{
        public:
            struct CreateInfo
            {
                const char *atlas_path;
                Syris::ShaderManager &shader_manager;
                Syris::OrthographicCameraController::CreateInfo camera_info;
                Syris::Statistics& statistics;
            };
            TriangleScene(CreateInfo info);
            ~TriangleScene();

            void on_update(const Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event* event)override;
        private:
            Syris::ShaderManager& m_shader_manager;
            Syris::OrthographicCameraController m_camera;
            uint32_t m_shader_id;
            glm::vec3 m_triangle_color;
            unsigned int m_vbo;
            unsigned int m_vao;

    };
}
