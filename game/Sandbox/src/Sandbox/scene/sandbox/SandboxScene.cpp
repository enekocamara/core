#include <array>
#include <format>
#include <vector>

#include <hpx/async.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/utils/glm_stdout.hpp"
#include "Syris/materials/Material.hpp"

#include "SandboxScene.hpp"
#include "Sandbox/shaders/sandbox_scene_layout.h"
#include "Sandbox/shaders/entity_shader.h"
#include "Sandbox/world_generator/world_generator.hpp"
#include "Sandbox/ecs/Components.h"
#include "Sandbox/ecs/Tile.h"
#include "Sandbox/ecs/Player.h"
#include "Sandbox/scene/helper.h"

namespace Sandbox{

    template<int size>
    Syris::Material* makeSimpleMaterial(Syris::ShaderManager& shader_manager, Syris::ShaderManager::ShaderID shader_id){
        //set up default data to fill buffers
        std::vector<TileVertices> vertices(size);
        std::vector<TileIndices> indices(size);
        std::vector<TileInstancedData> instanced_data(size);

        int span = 0;
        for (auto &indice : indices)
        {
            for (auto &num : indice.vertices)
                num += 4 * span;
            span++;
        }

        //Create attribute layouts that describe the interaction with shaders
        using AttCreateInfo = Syris::AttributeLayout::CreateInfo;

        //   per vertex
        AttCreateInfo layout_pos{
            .values_count = 2,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = false,
        };
        std::array<AttCreateInfo, 1> vertex_layouts_info = {layout_pos};

        //Attribute layout list combines more than one layout managing attribute indexing and 'span'
        Syris::AttributeLayoutList vertex_attribute_list({vertex_layouts_info.begin(), vertex_layouts_info.end()}, 0);


        //  per instance
        AttCreateInfo instanced_tex_coord{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true,
        };
        AttCreateInfo instanced_model_col_0{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true};
        AttCreateInfo instanced_model_col_1{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true};
        AttCreateInfo instanced_model_col_2{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true};
        AttCreateInfo instanced_model_col_3{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true
        };

        std::array<AttCreateInfo, 5> instanced_layouts_info = {instanced_tex_coord, instanced_model_col_0, instanced_model_col_1, instanced_model_col_2, instanced_model_col_3};
        Syris::AttributeLayoutList instance_attribute_list({instanced_layouts_info.begin(), instanced_layouts_info.end()}, vertex_attribute_list.attribute_size());


        //set the data to the buffers
        using BufferInfo = Syris::VertexBuffer::SubBufferInfo;
        BufferInfo vertex_buffer = BufferInfo{
            .layout_list = vertex_attribute_list,
            .size = sizeof(TileVertices) * size,
            .data = vertices.data(),
        };
        BufferInfo instance_buffer{
            .layout_list = instance_attribute_list,
            .size = sizeof(TileInstancedData) * size,
            .data = instanced_data.data()};

        std::array<BufferInfo, 2> buffers = {vertex_buffer, instance_buffer};
        Syris::VertexBuffer::CreateInfo vertex_buffer_info = Syris::VertexBuffer::CreateInfo{
            .dynamic = false,
            .buffers_info = {buffers.begin(), buffers.end()},
        };

        /*const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl";
        const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl";
        */
        Syris::IndexBuffer::CreateInfo index_buffer_info = Syris::IndexBuffer::CreateInfo{
            .indices_count = indices.size(),
            .indices = reinterpret_cast<uint32_t*>(indices.data()),
            .dynamic = false,
        };
        Syris::Material::CreateInfo material_create_info = Syris::Material::CreateInfo{
            .shader_manager = shader_manager,
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = size
        };
        return Syris::Material::create_material(material_create_info);
        //return new Syris::renderer::RenderBuffer(render_buffer_info);
    }

    Syris::Material* make_player(Syris::ShaderManager& shader_manager){
        TileVertices tile_vertices = TileVertices();
        entity_shader::RawData vertex_buffer_rd{
            .data = &tile_vertices,
            .size = sizeof(TileVertices),
        };

        TileInstancedData instance_data = TileInstancedData();
        instance_data.tex_coord = {texture::atlas::player_0.min, texture::atlas::player_0.max};
        instance_data.translation = glm::translate(instance_data.translation, glm::vec3(0.5,0.5,1.f));
        entity_shader::RawData instance_buffer_rd{
            .data = &instance_data,
            .size = sizeof(TileInstancedData),
        };
        
        TileIndices tile_indices = TileIndices();
        Syris::IndexBuffer::CreateInfo index_buffer_info{ 
            .indices_count = 6,
            .indices = tile_indices.vertices.data(),
            .dynamic = false
        };

        return entity_shader::make_entity_material(shader_manager, vertex_buffer_rd, instance_buffer_rd, index_buffer_info, 1);
        /*
        TileIndices indices = TileIndices(); 
        for (auto in : indices.vertices){
            std::cout << "indice-> " << in << '\n';
        }
        TileVertices vertices = TileVertices();
        for (int i = 0; i < vertices.vertices.size(); i += 2){
            std::cout << "vertice-> " << vertices.vertices[i] << ',' << vertices.vertices[i + 1] << '\n';
        }
        Syris::texture::Rectangle2D player_texture_rect = ecs::Player::defaultTextureBundle().src;
        player_texture_rect = texture::atlas::player_0;

        Syris::AttributeLayout::CreateInfo vertex_pos{
            .values_count = 2,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = false
        };
        std::vector<Syris::AttributeLayout::CreateInfo> layouts_pos{vertex_pos};
        Syris::AttributeLayoutList player_pos_layout({layouts_pos.begin(), layouts_pos.end()},0);

        Syris::AttributeLayout::CreateInfo tex_coord{
            .values_count = 4,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true
        };

        std::vector<Syris::AttributeLayout::CreateInfo> layouts_tex{tex_coord};
        Syris::AttributeLayoutList player_tex_layout({layouts_tex.begin(), layouts_tex.end()}, player_pos_layout.attribute_size());

        Syris::VertexBuffer::SubBufferInfo player_pos_subbuffer{
            .layout_list = player_pos_layout,
            .size = sizeof(TileVertices),
            .data = vertices.vertices.data(),
        };
        Syris::VertexBuffer::SubBufferInfo player_tex_subbuffer{
            .layout_list = player_tex_layout,
            .size = sizeof(Syris::texture::Rectangle2D),
            .data = &player_texture_rect,
        };

        std::vector<Syris::VertexBuffer::SubBufferInfo> buffers_info{player_pos_subbuffer, player_tex_subbuffer};

        Syris::VertexBuffer::CreateInfo vertex_buffer_info{
            .dynamic = true,
            .buffers_info = buffers_info
        };

        Syris::IndexBuffer::CreateInfo index_buffer_info{
            .indices_count = 6,
            .indices = reinterpret_cast<uint32_t*>(&indices),
            .dynamic = false,
        };
        Syris::Material::CreateInfo info{
            .shader_manager = shader_manager,
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = 1
        };
        return Syris::Material::create_material(info);
        */
    }

    SandboxScene::SandboxScene(CreateInfo info):m_registry(info.registry), m_texture_atlas(info.atlas_path), m_graphics_context(info.context), m_camera(info.camera_info){
        //const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl";
        //const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl";
        /*Syris::IShaderLayout* layout = new Syris::ShaderLayout<>();
        Syris::Shader::CreateInfo shader_info = {
            .path = "sandbox_scene"
        };*/
        Syris::Logger::client_info("sandbox scene being created");
        m_shader_id = m_graphics_context.get_shader_manager().add_shader(sandbox_scene::get_shader_info());
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }
        m_texture_atlas.init();
        bool a = true; 
        if (a){
            world_generator::generateGround<m_map_config>(m_registry);
            //m_buffer = makeSimpleRenderBuffer<m_map_config.num_tiles_x * m_map_config.num_tiles_y>();
            m_material = makeSimpleMaterial<m_map_config.num_tiles_x * m_map_config.num_tiles_y>(m_graphics_context.get_shader_manager(), m_shader_id);
            CHECK_GL_ERROR();
        }else{
            auto texture = ecs::Tile::defaultTextureBundle();
            texture.src = texture::atlas::grass_0;
            ecs::Tile::newTile(glm::vec2(0,0), texture, m_registry, ecs::CTile::TileType::Grass);
            //m_buffer = makeSimpleRenderBuffer<1>();
            CHECK_GL_ERROR();
        }
        ecs::MovementKeys keys =  ecs::MovementKeys{
            .up = GLFW_KEY_W,
            .down = GLFW_KEY_S,
            .left = GLFW_KEY_A,
            .right = GLFW_KEY_S,
        };
        Syris::texture::Texture2DBundle texture = ecs::Player::defaultTextureBundle();
        m_player_id = ecs::Player::newPlayer({0,0}, keys, texture, m_registry);

        /*
        uint32_t player_shader_id = m_graphics_context.get_shader_manager().add_shader(player_shader::get_shader_info());
        std::cout << "shader id player: " << player_shader_id << '\n';
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }*/
        m_player_material = make_player(m_graphics_context.get_shader_manager());
        
        update_data(false);
        Syris::Logger::client_info("sandbox scene successfully created"); 
        hpx::async(std::bind(&SandboxScene::sim_loop, this));
    }

    void SandboxScene::sim_loop(){
        using chrono_hrc = std::chrono::high_resolution_clock;
        m_sim_time.init_time = chrono_hrc::now();
        m_sim_time.last_frame = chrono_hrc::now();
        while(m_sim_loop_running){
            chrono_hrc::time_point now = chrono_hrc::now();
            m_sim_time.delta_time_ms =
                std::chrono::duration<float, std::milli>(now - m_sim_time.last_frame).count();
            auto [cPos, cDir, cSpeed] = m_registry.get<ecs::AsyncComponent<ecs::CPosition>, ecs::CDir, ecs::CSpeed>(m_player_id);
            cPos.set({cPos.get().pos + cDir.value * cSpeed.value * m_sim_time.delta_time_ms});
            std::cout << "Speed: " << cSpeed.value << '\n';
            m_sim_time.last_frame = now;
        }
    }

    void SandboxScene::update_data(bool imgui){
        //using material
        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
                                                               ecs::CPosition, ecs::CTile>);

        uint32_t index = 0;
        for (auto entity : render_group)
        {
            //get entity
            auto [cTexture, cPosition] =
                render_group.get<ecs::CTexture, ecs::CPosition>(entity);

            //get texture
            Syris::texture::Texture2D texture = m_texture_atlas.getTexture();

            //set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cPosition.pos, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
            
            //data to change
                //m_buffer->bind(1);
            TileInstancedData data;
            data.translation = model;
            data.tex_coord = {cTexture.rect.min, cTexture.rect.max};
            Syris::Material::DataSet data_set{
                .index_buffer = false,
                .sub_buffer_index = 1,
                .index = index * sizeof(TileInstancedData),
                .size = sizeof(TileInstancedData),
                .data = &data
            };
            m_material->set_data(data_set);
            ///glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(TileInstancedData), &data);
            CHECK_GL_ERROR();

            index++;
        }
        //without material
        /*
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
        */
    }
    void SandboxScene::on_update(Syris::engine_time::Time& time){
        //move camera
        //m_camera.on_update(time);

        //ecs::Player::animate(m_registry, m_player_id, time);

        //render shader hot reload
        m_graphics_context.get_shader_manager().get_shader(m_shader_id)->on_update(time);

        //draw material
        sandbox_scene::ShaderLayoutTuple data = {m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture()};
        m_material->draw(&data);


        ecs::AsyncComponent<ecs::CPosition>& cPos =  m_registry.get<ecs::AsyncComponent<ecs::CPosition>>(m_player_id);
        Syris::texture::Texture2D texture = m_texture_atlas.getTexture();
        // std::cout << "min: " << cTexture.rect.min << " max: " << cTexture.rect.max << '\n';
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(
            model, glm::vec3(cPos.get().pos,
                             0.0f)); // first translate (transformations are: scale
                                     // happens first, then rotation, and then final
                                     // translation happens; reversed order)

        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f)); // last scale

        ecs::CDir& dir = m_registry.get<ecs::CDir>(m_player_id);
        TileInstancedData instance_data = TileInstancedData();
        instance_data.tex_coord = {texture::atlas::player_0.min, texture::atlas::player_0.max};
        instance_data.translation = model;

        Syris::Material::DataSet data_set{
            .index_buffer = false,
            .sub_buffer_index = 1,
            .index = 0,
            .size = sizeof(TileInstancedData),
            .data = &instance_data, //set in every option or error
        };
        m_player_material->set_data(data_set);
        m_camera.getCamera().set_position({ cPos.get().pos, 0.f });
        entity_shader::ShaderLayoutTuple data_player = {m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture()};
        //player_shader::ShaderLayoutTuple data_player = {m_texture_atlas.getTexture()};
        m_player_material->draw(&data_player);
    }
    bool SandboxScene::on_event(Syris::Event* event){
        if (event->get_type() == Syris::EventType::KeyPressed){
            Syris::EventKey *event_key = (Syris::EventKey*)event; 
            int key = event_key->get_key();
            
            auto [cDir, cSpeed] = m_registry.get<ecs::CDir, ecs::CSpeed>(m_player_id);
            if (event_key->get_action()== GLFW_PRESS)
                cSpeed.value = 1.f;
            else if (event_key->get_action() == GLFW_RELEASE)
                cSpeed.value = 0.f;
            switch(key){ 
                case GLFW_KEY_W:
                    cDir.value = {0.f, 1.f}; 
                    break;
                case GLFW_KEY_S:
                    cDir.value = {0.f,-1.f}; 
                    break;
                case GLFW_KEY_A:
                    cDir.value = {-1.f,0.f}; 
                    break;
                case GLFW_KEY_D:
                    cDir.value = {1.f,0.f}; 
                    break;
            }
        }
        //Syris::Logger::client_info(std::format("event type {}", (int)event->get_type()).c_str());
        return false;
    }
    SandboxScene::~SandboxScene(){
        m_sim_loop_running = false;
    }
}

