#include "Test.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "Syris/app/OpenGLWindow.hpp"
#include "Syris/shader/Shader.hpp"
#include "Syris/renderer/batch_renderer/NewBatchRenderer.h"

// A sample function to test
int add(int a, int b) {
    return a + b;
}

// Tests
TEST(SampleTest, Addition) {
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(-1, 1), 0);
}

std::unique_ptr<Syris::Application> get_client_app(Syris::AppInit& app_init)
{
    float ratio = 1920.f / 1080.f;
    return std::make_unique<Test::TestApp>(app_init.statistics);
}


namespace  Test{
    TestApp::TestApp(Syris::Statistics& statistics):m_statistics(statistics){
        Syris::GraphicsContext::CreateInfo gc_info = {
            .layers_info = Syris::LayerManager::CreateInfo{
                .layers = {}},
            .window_info = Syris::Window::CreateInfo{
                .dimmensions = glm::ivec2(1920, 1080),
                .name = std::string("hola"),
                .vSync = true,
                .statistics = m_statistics
            },
            .statistics = statistics
        };
        m_graphics_context = Syris::GraphicsContext::new_context(gc_info);
        Syris::Shader::CreateInfo shader_info{
            .path = "quad",
        };
        auto shader_id_res = m_graphics_context->get_shader_manager().add_shader(shader_info);
        if (!shader_id_res) {
            CLIENT_ERROR(std::format("Failed to create shader:, {}", shader_id_res.error()));
            throw std::runtime_error(shader_id_res.error());
        }
        
        m_material_manager = std::make_unique<Syris::BatchRendererManager>(Syris::BatchRendererManager::CreateInfo{statistics});
        
        TileIndices indices = TileIndices();

        Syris::IndexBuffer::CreateInfo index_buffer_info{
            .indices_count = 6,
            .indices = indices.vertices.data(),
            .dynamic = false,
        };

        Syris::BatchRendererLayout::CreateInfo entity_RL_info{
            .attributes_layout = m_graphics_context->get_shader_manager().get_shader(shader_id_res.value())->get_attribute_layout(),
            .uniforms_layout = m_graphics_context->get_shader_manager().get_shader(shader_id_res.value())->get_uniform_layout(),
            .statistics = statistics,
        };
        std::cout << "attributes:\n";
        entity_RL_info.attributes_layout.debug();
        std::cout << "uniforms:\n";
        entity_RL_info.uniforms_layout.debug();
        std::cout << "done\n";
        
        std::unique_ptr<Syris::BatchRendererLayout> layout = std::make_unique<Syris::BatchRendererLayout>(entity_RL_info);
        std::cout << "layout done\n";
        Syris::VertexBuffer::SubBufferInfo pos(false, false);
        pos.push({"aPos", Syris::Type::vec2});
        
        Syris::VertexBuffer::SubBufferInfo instance(true, false);
        instance.push({"aModel", Syris::Type::mat4});
        instance.push({"color", Syris::Type::vec3});

        layout->set_subbuffer(pos);
        layout->set_subbuffer(instance);
        auto res = layout->finish();
        if (!res){
            CLIENT_ERROR(std::format("Not all attributes were set 2: [{}]", res.error()));
            exit(1);
        }
        std::cout << "layout finished\n";
        TileVertices vertices = TileVertices();
        Syris::BatchRenderer::CreateInfo material_info{
            .name = "test entity renderer",
            .shader_manager = m_graphics_context->get_shader_manager(),
            .shader_id = shader_id_res.value(),
            .index_buffer_info = index_buffer_info,
            .layout = std::move(layout),
            .statistics = m_statistics,
        };
        m_batch_renderer = Syris::new_batch_renderer<QuadData>(material_info);
        
        
        m_graphics_context->get_layer_manager().push_layer(this);
        m_graphics_context->get_layer_manager().push_layer(&m_statistics);
        m_statistics.add_child(m_statistics.get_root(), m_batch_renderer->get_statistics());
        Syris::BR_SetAttributeRequest request{
            .values = {vertices.vertices.begin(), vertices.vertices.end()}
        };
        m_batch_renderer->set_attribute(request);
        
    }
    void TestApp::run(){
        Syris::engine_time::Time time;
        time.start();
        
        QuadData mem_data = { glm::scale(glm::mat4(1.f), glm::vec3(0.5f)),
                         glm::vec3(1.f,0.f,1.f) };
        std::array<std::pair<std::size_t, void*>, 1> data;
        data[0] = { 0, &mem_data };
        Syris::BR_RequestSparse player{
            .entity = (entt::entity)0,
            .data = {data.begin(), data.end()},
        };
        m_batch_renderer->add_entity(player);
        while(!m_graphics_context->should_window_close()){
            time.next_frame();
            m_graphics_context->on_update(time);
        }
    }
    void TestApp::on_update(const Syris::engine_time::Time &time){
        m_batch_renderer->draw(nullptr);
    }
    bool TestApp::on_event(Syris::Event *event) {
        return false;
    }
}