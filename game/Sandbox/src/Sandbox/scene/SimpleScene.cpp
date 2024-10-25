#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SimpleScene.hpp"

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/shader/Shader.hpp"
#include "Syris/statistics/Components.hpp"

#include "../world_generator/world_generator.hpp"
#include "../ecs/Components.h"
#include "../ecs/Tile.h"
#include "../shaders/simple_scene_layout.h"
#include "helper.h"
#include "../shaders/quad_shader.h"
#include "Sandbox/ecs/Quad.h"

namespace Sandbox{
    Syris::MaterialManager::MaterialID make_simple_material(Syris::MaterialManager& material_manager, Syris::ShaderManager& shader_manager, Syris::Statistics& statistics){
        uint32_t shader_id = shader_manager.add_shader(quad_shader::get_shader_info());
        if (!shader_id){
            throw std::runtime_error("failed to add shader");
        }

        TileIndices indices = TileIndices();
        TileVertices vertices = TileVertices();

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


        AttCreateInfo layout_color{
            .values_count = 3,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = false,
        };
        // per vertex layout list
        std::array<AttCreateInfo, 1> instance_layouts_info = {layout_color};
        Syris::AttributeLayoutList instance_attribute_list({instance_layouts_info.begin(), instance_layouts_info.end()}, vertex_attribute_list.attribute_size());

        // define vertex and instance buffer info for render buffer:
        SubBufferInfo subbufer_color{
            .layout_list = instance_attribute_list,
            .size = 0,
            .data = nullptr,
        };

        std::array<SubBufferInfo,2> subbuffers= {subbufer_pos, subbufer_color};
        Syris::VertexBuffer::CreateInfo vertex_buffer_info{
            .dynamic = false,
            .buffers_info = {subbuffers.begin(), subbuffers.end()},
            .statistics = statistics
        };

        Syris::IndexBuffer::CreateInfo index_buffer_info{
            .indices_count = 6,
            .indices = (uint32_t*)&indices,
            .dynamic = false
        };

        Syris::Material::CreateInfo material_info{
            .name = "Simple scene material",
            .shader_manager = shader_manager, 
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = 0,
            .statistics = statistics
        };
        return material_manager.add_material<TileInstancedData>(material_info);
    }
    SimpleScene::SimpleScene(CreateInfo info) : m_graphics_context(info.context), m_material_manager({ info.statistics }), m_entity_manager({ m_material_manager }), m_camera(info.camera_info) {
        //const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\simple_vertex_shader.glsl";
        //const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\simple_fragment_shader.glsl";
        Syris::Statistics::AddModuleInfo mod_info{};
        auto id = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CScene>(id, "Simple Scene");
        
        Syris::Logger::client_info("simple scene being created");
        m_shader_id = m_graphics_context.get_shader_manager().add_shader(simple_scene::get_shader_info());
        if (!m_shader_id){
             throw std::runtime_error("failed to add shader");
        }
        auto texture = ecs::Tile::defaultTextureBundle();
        texture.src = texture::atlas::grass_0;
        //ecs::Tile::newTile(glm::vec2(0, 0), texture, m_registry, ecs::CTile::TileType::Grass);
        m_material = make_simple_material(m_material_manager, m_graphics_context.get_shader_manager(), info.statistics);
        ecs::Quad::newQuad({1.f,0.f,0.f}, m_entity_manager, m_material);
        Syris::Logger::client_info("simple scene successfully created");
        CHECK_GL_ERROR();
    }
    void SimpleScene::on_update(Syris::engine_time::Time& time){
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
        m_material_manager.draw(m_material, nullptr);
        /*
        simple_scene::ShaderLayoutTuple uniforms = m_camera.getCamera().get_view_projection_matrix();
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
