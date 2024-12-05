#pragma once
#include <gtest/gtest.h>
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/layers/Layer.hpp"
#include "Syris/app/Application.hpp"
namespace Test{
    struct TileIndices{
        std::array<uint32_t, 6> vertices;
        TileIndices(){
            vertices = {
                0,1,2,2,3,0
            };
        }
    };
    struct TileVertices{
        std::array<float, 8> vertices;
        TileVertices(){
            vertices = {
                // pos     //uv texture
                -1.f, 1.f,//  0.0f, 1.0f,// top-left
                1.f,  1.f,//  1.0f, 1.0f,// top-right
                1.f,  -1.f,// 1.0f, 0.0f,// bottom-right
                -1.f, -1.f,// 0.0f, 0.0f,// bottom-left
            };
        }
        std::pair<glm::vec2, glm::vec2> get_min_max(){
            return {{vertices[2], vertices[6]},{vertices[11],vertices[3]}}; 
        }

    };
    struct alignas(16) QuadData{
        glm::mat4 model;
        glm::vec3 color;
    };

    struct Player{
        glm::mat4 model;
        glm::vec3 mat;
    };
    class TestApp : public Syris::Application, public Syris::Layer{
        public:
            TestApp(Syris::Statistics& statistics);
            void run() override;
            void on_update(const Syris::engine_time::Time& time)override;
            bool on_event(Syris::Event* event)override;
            Syris::Statistics& m_statistics;
            std::unique_ptr<Syris::GraphicsContext> m_graphics_context;
            std::unique_ptr<Syris::BatchRendererManager> m_material_manager;
            std::unique_ptr<Syris::BatchRenderer> m_batch_renderer;
            Syris::BatchRendererManager::BR_ID m_bath_renderer_id;
    };
}