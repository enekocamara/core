#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SimpleScene.hpp"

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/shader/Shader.hpp"
#include "Syris/statistics/Components.hpp"

//#include "../world_generator/world_generator.hpp"
#include "../ecs/Components.h"
#include "../ecs/Tile.h"
#include "../shaders/simple_scene_layout.h"
#include "helper.h"
#include "../shaders/quad_shader.h"
#include "Sandbox/ecs/Quad.h"

namespace Sandbox{
    void SimpleScene::make_batch_renderer(){
        TileIndices indices = TileIndices();
        TileVertices vertices = TileVertices();

        /*
        using AttCreateInfo = Syris::AttributeLayout::CreateInfo;
        // define per vertex layout
        AttCreateInfo layout_pos{
            .values_count = 2,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = false,
        };
        // per vertex layout list
        std::array<AttCreateInfo, 1> vertex_layouts_info = {layout_pos};
        Syris::AttributeLayoutList vertex_attribute_list({vertex_layouts_info.begin(), vertex_layouts_info.end()}, 0);

        using SubBufferInfo = Syris::VertexBuffer::SubBufferInfo;
        // define vertex and instance buffer info for render buffer:
        SubBufferInfo subbufer_pos{
            .layout_list = vertex_attribute_list,
            .size = sizeof(vertices),
            .data = vertices.vertices.data(),
        };
        */

        Syris::VertexBuffer::SubBufferInfo pos(false, false);
        pos.push({"aPos", Syris::Type::vec2});

        Syris::VertexBuffer::SubBufferInfo instance(true, false);
        instance.push({"aModel", Syris::Type::mat4});

        std::unique_ptr<Syris::BatchRendererLayout> layout = std::make_unique<Syris::BatchRendererLayout>(m_shader_manager.get_shader(m_shader_id), m_statistics);
        layout->set_subbuffer(pos);
        layout->set_subbuffer(instance);
        auto res = layout->finish();
        if (!res)
        {
            throw std::runtime_error(std::format("failed to create ER layout: {}", res.error()));
        }

        Syris::IndexBuffer::CreateInfo index_buffer_info{
            .indices_count = 6,
            .indices = (uint32_t *)&indices,
            .dynamic = false
        };

        Syris::BatchRenderer::CreateInfo info{
            .name = "simple scene",
            .shader_manager = m_shader_manager,
            .shader_id = m_shader_id,
            .index_buffer_info = index_buffer_info,
            .layout = std::move(layout),
            .statistics = m_statistics,
        };
        m_batch_renderer_id = m_batch_renderer_manager.add_renderer<glm::mat4>(info);
        Syris::BR_SetAttributeRequest positions{
            .values = {vertices.vertices.begin(),vertices.vertices.end() },
        };
        m_batch_renderer_manager.get_renderer(m_batch_renderer_id)->set_attribute(positions);
    }
    SimpleScene::SimpleScene(CreateInfo info)
        : m_shader_manager(info.shader_manager),
        m_batch_renderer_manager({ info.statistics }),
        m_entity_manager({ m_batch_renderer_manager }),
        m_statistics(info.statistics),
        m_camera(info.camera_info) {
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&SimpleScene::render_statistics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod_ID = info.statistics.add_module(mod_info);

        info.statistics.get_registry().emplace<Syris::statistics::CScene>(m_statistic_mod_ID, "Simple Scene");

        Syris::Logger::client_info("simple scene being created");
        Syris::Shader::CreateInfo shader_info{
            .path = "simple_scene"
        };
        auto shader_id_res = m_shader_manager.add_shader(shader_info);
        if (!shader_id_res) {
            throw std::runtime_error(std::format("Failed to add simple scene shader: {}", shader_id_res.error()));
        }
        Syris::Logger::client_info("shader compiled");
        m_shader_id = shader_id_res.value();
        make_batch_renderer();
        Syris::Logger::client_info("batch renderer made");
        glm::mat4 mem_data(1.f);
        std::array<std::pair<std::size_t, void*>, 1> data;
        data[0] = { 0, &mem_data };
    
        Syris::BR_RequestSparse quad{
            .entity =(entt::entity)0,
            .data = {data.begin(), data.end()}
        };
        m_batch_renderer_manager.get_renderer(m_batch_renderer_id)->add_entity(quad); 
        Syris::Logger::client_info("simple scene successfully created");
        CHECK_GL_ERROR();
    }
    void SimpleScene::on_update(const Syris::engine_time::Time& time){
        //it now owns the camera
        /*outdated by renderbuffer??*/
        /*
        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
                ecs::CPosition, ecs::CTile>);
        uint32_t index = 0;
        for (auto entity : render_group) {
            auto [cTexture, cPosition] =
                render_group.get<ecs::CTexture, ecs::CPosition>(entity);
            Syris::texture::Texture2D texture = m_texture_atlas.getTexture();
            //std::cout << "min: " << cTexture.rect.min << " max: " << cTexture.rect.max << '\n';
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                    model, glm::vec3(cPosition.pos,
                        0.0f)); // first translate (transformations are: scale
                                // happens first, then rotation, and then final
                                // translation happens; reversed order)

            
            //   model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y,
             //  0.0f)); // move origin of rotation to center of quad model =
             //  glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); //
             //  then rotate model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f
            //  size.y, 0.0f)); // move origin back
            
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f)); // last scale
            
            CHECK_GL_ERROR();
            m_buffer->bind(0);
            float updated_vertices[] = { 
                // pos     //uv texture 
                -1.0f, 1.0f,  cTexture.rect.min.x, cTexture.rect.max.y, // top-left
                1.0f,  1.0f,  cTexture.rect.max.x, cTexture.rect.max.y, // top-right
                1.0f,  -1.0f, cTexture.rect.max.x, cTexture.rect.min.y, // bottom-right
                -1.0f, -1.0f, cTexture.rect.min.x, cTexture.rect.min.y  // bottom-left
            };
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileVertices), sizeof(TileVertices), updated_vertices );
            CHECK_GL_ERROR();
            m_buffer->bind(1);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(glm::mat4), &model );
            index++;
            CHECK_GL_ERROR();
            
        }
        m_buffer->bind(0);
        //glUseProgram(program);already done by graphics context on update
        //float buffer[1000];
        //glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
        //TileVertices * vertices = reinterpret_cast<TileVertices*>(buffer);
        //int i = 0;
        //std::cout << "after\n\n\n";
        //while (i < 1000 / 16){
        //    std::cout << "min : " << vertices[1].get_min_max().first << "max : " << vertices[i].get_min_max().second << '\n';
        //    i++;
        //};
        */

        //Syris::Shader* shader = m_graphics_context.get_shader_manager().get_shader(m_shader_id);
        glm::mat4 cam = m_camera.getCamera().get_view_projection_matrix();
        Syris::Uniform camera_uniform{
            .name = "ViewProjection",
            .data = &cam,
            .pnext = nullptr,
        };
        m_batch_renderer_manager.draw(m_batch_renderer_id, &camera_uniform);
        
        /*
        simple_scene::ShaderLayoutTuple uniforms = 
        m_graphics_context.get_shader_manager().use_shader(m_shader_id, &uniforms);
        m_buffer->bind(0); //m_vertexBuffer->bind(1); m_indexBuffer->bind();
        
        //shader->set_uniform_value(m_camera.getCamera().get_view_projection_matrix(), "ViewProjection");
//        shader->set_uniform_value(glm::mat4(1.f), "ViewProjection");
        
        //renderAPI::set_uniform_value(context_opengl->get_program(), projection_view, "ViewProjection");
        //glActiveTexture(GL_TEXTURE0);!!TODO

        //shader->set_uniform(glm::vec3(1.f), "spriteColor");
        //renderAPI::set_uniform_value(context_opengl->get_program(), glm::vec3(1.f), "spriteColor");
        CHECK_GL_ERROR();
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
        CHECK_GL_ERROR();
        //Syris::Logger::client_info("drawing...\n");
        //m_buffer->draw_buffer(m_graphics_context.get_shader_manager().get_shader(m_shader_id), m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture(), 100 * 100);
        */
    }
    bool SimpleScene::on_event(Syris::Event* event){
        return false;
    }
    SimpleScene::~SimpleScene(){

    }
}
