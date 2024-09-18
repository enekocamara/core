#include "SandboxScene.hpp"
#include <array>
#include "../world_generator/world_generator.hpp"
#include "../ecs/Components.h"
#include "../ecs/Tile.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Syris/platform/OpenGl/OpenGLErrors.hpp>
#include "Syris/utils/glm_stdout.hpp"
#include <format>
namespace Sandbox{

    struct TileVertices{
        std::array<float, 8> vertices;
        TileVertices(){
            vertices = {
                // pos     //uv texture
                -1.0f, 1.0f,//  0.0f, 1.0f,// top-left
                1.0f,  1.0f,//  1.0f, 1.0f,// top-right
                1.0f,  -1.0f,// 1.0f, 0.0f,// bottom-right
                -1.0f, -1.0f,// 0.0f, 0.0f,// bottom-left
            };
        }
        std::pair<glm::vec2, glm::vec2> get_min_max(){
            return {{vertices[2], vertices[6]},{vertices[11],vertices[3]}}; 
        }

    };
    struct TileIndices{
        std::array<uint32_t, 6> vertices;
        TileIndices(){
            vertices = {
                0,1,2,2,3,0
            };/*
            int index = 49 * 4;
            
            for (auto& num : vertices){
                num += index;
            }*/
        }
    };
    struct TileInstancedData{
        glm::vec4 tex_coord = {texture::atlas::grass_0.min, texture::atlas::grass_0.max};
        glm::mat4 translation = glm::mat4(1.f);
    };


    template<int size>
    Syris::renderer::RenderBuffer* makeSimpleRenderBuffer(){
            std::vector<TileVertices> vertices(size);
            std::vector<TileIndices> indices(size);
            int span = 0;
            for (auto& indice : indices){
                for (auto& num : indice.vertices)
                    num += 4 * span;
                span++;
            }
            std::vector<TileInstancedData> instanced_data(size);

            using AttCreateInfo = Syris::renderer::AttributeLayout::CreateInfo;
            AttCreateInfo layout_pos{
                .values_count = 2,
                .value_type= Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = false,
            };/*
            AttCreateInfo layout_tex{
                .values_count = 2,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = false,
            };*/
            std::array<AttCreateInfo,1> vertex_layouts_info =  {layout_pos}; 
            Syris::renderer::AttributeLayoutList vertex_attribute_list({vertex_layouts_info.begin(), vertex_layouts_info.end()}, 0); 
 
            AttCreateInfo instanced_tex_coord{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true,
            };
            AttCreateInfo instanced_model_col_0{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_1{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_2{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_3{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };

            std::array<AttCreateInfo, 5> instanced_layouts_info =  {instanced_tex_coord, instanced_model_col_0, instanced_model_col_1, instanced_model_col_2, instanced_model_col_3}; 
            Syris::renderer::AttributeLayoutList instance_attribute_list({instanced_layouts_info.begin(), instanced_layouts_info.end()}, vertex_attribute_list.attribute_size()); 

            using BufferInfo = Syris::renderer::VertexBuffer::BufferInfo;
            BufferInfo vertex_buffer{
                .layout_list = vertex_attribute_list,
                .size = sizeof(TileVertices) * size,
                .data = vertices.data(),
            };
            BufferInfo instance_buffer{
                .layout_list = instance_attribute_list,
                .size = sizeof(TileInstancedData) * size,
                .data = instanced_data.data()
            };

            Syris::renderer::RenderBuffer::CreateInfo render_buffer_info{
                .dynamic = true,
                .index_count = 6,
                .indices = reinterpret_cast<uint32_t *>(indices.data()),
                .per_vertex_buffer_info = vertex_buffer,
                .per_instance_buffer_info = instance_buffer,
            };
            return new Syris::renderer::RenderBuffer(render_buffer_info);
    }
    
    SandboxScene::SandboxScene(CreateInfo info):m_registry(info.registry), m_texture_atlas(info.atlas_path), m_graphics_context(info.context), m_camera(info.camera_info){
        const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl";
        const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl";
        m_shader_id = m_graphics_context.get_shader_manager().add_shader({fragment_shader_path, vertex_shader_path});
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }
        m_texture_atlas.init();
        bool a = true; 
        if (a){
            world_generator::generateGround<m_map_config>(m_registry);
            m_buffer = makeSimpleRenderBuffer<m_map_config.num_tiles_x * m_map_config.num_tiles_y>();
            CHECK_GL_ERROR();
        }else{
            auto texture = ecs::Tile::defaultTextureBundle();
            texture.src = texture::atlas::grass_0;
            ecs::Tile::newTile(glm::vec2(0,0), texture, m_registry, ecs::CTile::TileType::Grass);
            m_buffer = makeSimpleRenderBuffer<1>();
            CHECK_GL_ERROR();
        }
        update_data(false);
    }
    void SandboxScene::update_data(bool imgui){
        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
                                                               ecs::CPosition, ecs::CTile>);
        uint32_t index = 0;
        bool first = false;

        if (imgui){
            ImGui::Begin("Map Debug");
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            ImGui::BeginTable("table1", 10, flags);
            for (int i = 0; i < 10; i++)
            {
                ImGui::TableSetupColumn(std::format("{}", i).c_str());
            }
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
        }
        int row = 0;
        int column = 0;
        for (auto entity : render_group)
        {
            auto [cTexture, cPosition] =
                render_group.get<ecs::CTexture, ecs::CPosition>(entity);
            Syris::texture::Texture2D texture = m_texture_atlas.getTexture();
            // std::cout << "min: " << cTexture.rect.min << " max: " << cTexture.rect.max << '\n';
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

            if (false) // vertex
            {
                CHECK_GL_ERROR();
                m_buffer->bind(0);
                std::array<float, 16> updated_vertices = {
                    // pos     //uv texture
                    -1.0f, 1.0f, cTexture.rect.min.x, cTexture.rect.max.y, // top-left
                    1.0f, 1.0f, cTexture.rect.max.x, cTexture.rect.max.y,  // top-right
                    1.0f, -1.0f, cTexture.rect.max.x, cTexture.rect.min.y, // bottom-right
                    -1.0f, -1.0f, cTexture.rect.min.x, cTexture.rect.min.y // bottom-left
                };
                if (first)
                {
                    updated_vertices = {
                        // pos     //uv texture
                        -1.0f, 1.0f, 0, 0,  // top-left
                        1.0f, 1.0f, 0, 0,   // top-right
                        1.0f, -1.0f, 0, 0,  // bottom-right
                        -1.0f, -1.0f, 0, 0, // bottom-left
                    };
                    TileVertices *ref = reinterpret_cast<TileVertices *>(updated_vertices.data());
                    std::cout << "FIRST min : " << ref->get_min_max().first << "max : " << ref->get_min_max().second << '\n';
                    first = false;
                }
                // std::cout << "min: " << cTexture.rect.min.y << '\n';
                glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileVertices), sizeof(TileVertices), updated_vertices.data());
                CHECK_GL_ERROR();
            }
            m_buffer->bind(1);
            TileInstancedData data;
            data.translation = model;
            data.tex_coord = {cTexture.rect.min, cTexture.rect.max};
            //               if (index % 2 == 0){
            //                  data.tex_coord = {texture::atlas::dirt_0.min, texture::atlas::dirt_0.max};
            // s              }
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(TileInstancedData), &data);
            CHECK_GL_ERROR();

            // im gui table
            column = index % 10;
            row = index / 10;
            if (imgui)
            {
                if (index % 10 == 0)
                {
                    ImGui::TableNextRow();
                }
                ImGui::TableSetColumnIndex(column);
                char buf[32];
                sprintf(buf, "%d", index);
                ImGui::TextUnformatted(buf);
            }
            index++;
        }
        if (imgui){
            ImGui::EndTable();
            ImGui::End();
        }
        m_buffer->bind(0);
        if (false)
        {
            // glUseProgram(program);already done by graphics context on update
            std::array<TileVertices, 100> vertices;
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TileVertices) * vertices.size(), vertices.data());
            std::cout << "after\n\n\n";
            for (auto &tile : vertices)
            {
                std::cout << "min : " << tile.get_min_max().first << "max : " << tile.get_min_max().second << '\n';
            };
        } 
    }
    void SandboxScene::on_update(Syris::engine_time::Time& time){
        //it now owns the camera
        /*outdated by renderbuffer??*/
        m_camera.on_update(time);
        m_graphics_context.get_shader_manager().get_shader(m_shader_id)->on_update(time);
        Syris::Shader* shader = m_graphics_context.get_shader_manager().get_shader(m_shader_id);
        shader->use();

        //update_data(true);

        /*if (true){
            auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
                                                                   ecs::CPosition, ecs::CTile>);
            uint32_t index = 0;
            bool first = false;

            ImGui::Begin("Map Debug");
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            ImGui::BeginTable("table1", 10, flags);
            for (int i = 0; i < 10; i++){
                ImGui::TableSetupColumn(std::format("{}", i).c_str());
            }
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            int row = 0;
            int column = 0;
            for (auto entity : render_group)
            {
                auto [cTexture, cPosition] =
                    render_group.get<ecs::CTexture, ecs::CPosition>(entity);
                Syris::texture::Texture2D texture = m_texture_atlas.getTexture();
                // std::cout << "min: " << cTexture.rect.min << " max: " << cTexture.rect.max << '\n';
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

                if (false) // vertex
                {
                    CHECK_GL_ERROR();
                    m_buffer->bind(0);
                    std::array<float, 16> updated_vertices = {
                        // pos     //uv texture
                        -1.0f, 1.0f, cTexture.rect.min.x, cTexture.rect.max.y, // top-left
                        1.0f, 1.0f, cTexture.rect.max.x, cTexture.rect.max.y,  // top-right
                        1.0f, -1.0f, cTexture.rect.max.x, cTexture.rect.min.y, // bottom-right
                        -1.0f, -1.0f, cTexture.rect.min.x, cTexture.rect.min.y // bottom-left
                    };
                    if (first)
                    {
                        updated_vertices = {
                            // pos     //uv texture
                            -1.0f, 1.0f, 0, 0,  // top-left
                            1.0f, 1.0f, 0, 0,   // top-right
                            1.0f, -1.0f, 0, 0,  // bottom-right
                            -1.0f, -1.0f, 0, 0, // bottom-left
                        };
                        TileVertices *ref = reinterpret_cast<TileVertices *>(updated_vertices.data());
                        std::cout << "FIRST min : " << ref->get_min_max().first << "max : " << ref->get_min_max().second << '\n';
                        first = false;
                    }
                    // std::cout << "min: " << cTexture.rect.min.y << '\n';
                    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileVertices), sizeof(TileVertices), updated_vertices.data());
                    CHECK_GL_ERROR();
                }
                m_buffer->bind(1);
                TileInstancedData data;
                data.translation = model;
                data.tex_coord = {cTexture.rect.min, cTexture.rect.max};
 //               if (index % 2 == 0){
  //                  data.tex_coord = {texture::atlas::dirt_0.min, texture::atlas::dirt_0.max};
  //s              _}
                glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(TileInstancedData), &data);
                CHECK_GL_ERROR();

                //im gui table
                column = index % 10;
                row = index / 10;
                if (index % 10 == 0){
                    ImGui::TableNextRow();
                }
                ImGui::TableSetColumnIndex(column);
                char buf[32];
                sprintf(buf, "%d", index);
                ImGui::TextUnformatted(buf);
                index++;
            }
            ImGui::EndTable();
            ImGui::End();
            m_buffer->bind(0);
            if (false){
                // glUseProgram(program);already done by graphics context on update
                std::array<TileVertices, 100>vertices;
                glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TileVertices) * vertices.size(), vertices.data());
                std::cout << "after\n\n\n";
                for (auto& tile : vertices)
                {
                    std::cout << "min : " << tile.get_min_max().first << "max : " << tile.get_min_max().second << '\n';
                };
            }
        }
        */


        //OpenGLContext* context_opengl = reinterpret_cast<OpenGLContext*>(context);
        //shader->use(); -> done
        m_buffer->bind(1);
        shader->set_uniform_value(m_camera.getCamera().get_view_projection_matrix(), "ViewProjection");
        CHECK_GL_ERROR();
        m_texture_atlas.getTexture().bind();
        CHECK_GL_ERROR();
        shader->set_uniform1i(0,"texture1");
        CHECK_GL_ERROR();
//        glUniform1i(glGetUniformLocation(context_opengl->get_program(), "texture1"), 0);
        // shader->set_uniform(glm::vec3(1.f), "spriteColor");
        //renderAPI::set_uniform_value(context_opengl->get_program(), glm::vec3(1.f), "spriteColor");
        CHECK_GL_ERROR();
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_map_config.num_tiles_x * m_map_config.num_tiles_y);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);// m_map_config.num_tiles_x * m_map_config.num_tiles_y);
        CHECK_GL_ERROR();
    }
    bool SandboxScene::on_event(Syris::Event* event){
        if (event->get_type() == Syris::EventType::KeyPressed){
            Syris::EventKey *event_key = (Syris::EventKey*)event; 
            int key = event_key->get_key();
            /*switch(key){
                case GLFW_KEY_W:
                    m_camera.on_event();
            }*/
        }
        //Syris::Logger::client_info(std::format("event type {}", (int)event->get_type()).c_str());
        return false;
    }
    SandboxScene::~SandboxScene(){

    }
}

