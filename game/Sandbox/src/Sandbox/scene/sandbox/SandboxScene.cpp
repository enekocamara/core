#include <array>
#include <format>
#include <vector>

#include <hpx/async.hpp>
#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/utils/glm_stdout.hpp"
#include "Syris/materials/MaterialManager.hpp"
#include "Syris/statistics/Components.hpp"

#include "SandboxScene.hpp"
#include "Sandbox/shaders/sandbox_scene_layout.h"
#include "Sandbox/shaders/entity_shader.h"
#include "Sandbox/world_generator/world_generator.hpp"
#include "Sandbox/ecs/Components.h"
#include "Sandbox/ecs/Tile.h"
#include "Sandbox/ecs/Bush.h"
#include "Sandbox/ecs/Player.h"
#include "Sandbox/scene/helper.h"

namespace Sandbox{

    void render_statistics(entt::entity entity, entt::registry& registry){
        Syris::statistics::render_childs(entity, registry);
    }
    //creates the empty material where all the entities will reside
    Syris::MaterialManager::MaterialID make_entity_material(Syris::EntityManager& entity_manager, Syris::ShaderManager& shader_manager, Syris::Statistics& statistics){
        
        TileVertices tile_vertices = TileVertices();
        entity_shader::RawData vertex_buffer_rd{
            .data = &tile_vertices,
            .size = sizeof(TileVertices),
        };

        TileInstancedData instance_data = TileInstancedData();
        instance_data.tex_coord = {texture::atlas::player_0.min, texture::atlas::player_0.max};
        instance_data.translation = glm::translate(instance_data.translation, glm::vec3(0.5,0.5,1.f));
        /*entity_shader::RawData instance_buffer_rd{
            .data = &instance_data,
            .size = sizeof(TileInstancedData),
        };*/
        entity_shader::RawData instance_buffer_rd{
            .data = nullptr,
            .size = 0,
        };
        
        TileIndices tile_indices = TileIndices();
        Syris::IndexBuffer::CreateInfo index_buffer_info{ 
            .indices_count = 6,
            .indices = tile_indices.vertices.data(),
            .dynamic = false
        };
/*
        {

            Syris::ShaderManager::ShaderID shader_id = shader_manager.add_shader(entity_shader::get_shader_info());
            if (shader_id == 0)
            {
                CLIENT_ERROR("failed to make instance material");
                exit(1);
            }
            using AttCreateInfo = Syris::AttributeLayout::CreateInfo;

            //   per vertex
            AttCreateInfo layout_pos{
                .values_count = 2,
                .value_type = Syris::ValueType::Float,
                .normalize = false,
                .perInstance = false,
            };
            std::array<AttCreateInfo, 1> vertex_layouts_info = {layout_pos};

            // Attribute layout list combines more than one layout managing attribute indexing and 'span'
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
                .perInstance = true};

            std::array<AttCreateInfo, 5> instanced_layouts_info = {instanced_tex_coord, instanced_model_col_0, instanced_model_col_1, instanced_model_col_2, instanced_model_col_3};
            Syris::AttributeLayoutList instance_attribute_list({instanced_layouts_info.begin(), instanced_layouts_info.end()}, vertex_attribute_list.attribute_size());

            // set the data to the buffers
            using BufferInfo = Syris::VertexBuffer::SubBufferInfo;
            BufferInfo vertex_buffer = BufferInfo{
                .layout_list = vertex_attribute_list,
                .size = vertex_buffer_rd.size, // sizeof(TileVertices) * size,
                .data = vertex_buffer_rd.data,
            };
            BufferInfo instance_buffer{
                .layout_list = instance_attribute_list,
                .size = instance_buffer_rd.size, // sizeof(TileInstancedData) * size,
                .data = instance_buffer_rd.data  // instanced_data.data()
            };

            std::array<BufferInfo, 2> buffers = {vertex_buffer, instance_buffer};
            Syris::VertexBuffer::CreateInfo vertex_buffer_info = Syris::VertexBuffer::CreateInfo{
                .dynamic = false,
                .buffers_info = {buffers.begin(), buffers.end()},
            };



            Syris::Material::CreateInfo info{
                .shader_manager = shader_manager,
                .shader_id = shader_id,
                .vertex_buffer_info = vertex_buffer_info,
                .index_buffer_info = index_buffer_info,
                .instance_count = 1,
            };
            return entity_manager.get_materials().add_material(info);
        }*/

        //check that the info returned by get entity material persists and was not freed
        Syris::Material::CreateInfo info = entity_shader::get_entity_material(shader_manager, vertex_buffer_rd, instance_buffer_rd, index_buffer_info, 0, statistics);
        //todo check all the values are actually initialized here
        auto a = entity_manager.get_materials().add_material<TileInstancedData>(info);
        return a;
    }

    SandboxScene::SandboxScene(CreateInfo info) :
        m_material_manager({info.statistics}),
        m_entity_manager({ m_material_manager}),
        m_texture_atlas(info.atlas_path),
        m_graphics_context(info.context),
        m_camera(info.camera_info),
        m_statistics(info.statistics),
        m_sim_fps({"simulation"})
        {

        //statistics
        Syris::Statistics::AddModuleInfo mod_info{
            .render = render_statistics,
        };
        m_statistic_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CScene>(m_statistic_mod_ID, "Sandbox Scene");

        info.statistics.add_child(m_statistic_mod_ID, m_material_manager.get_statistics());


        Syris::Logger::client_info("sandbox scene being created");
        m_texture_atlas.init();
        World::CreateInfo world_info{
            .m_material_manager = m_material_manager,
            .m_shader_manager = m_graphics_context.get_shader_manager(),
            .m_entity_manager = m_entity_manager,
            .world_dimmensions = {100,100},
            .statistics = info.statistics
        };
        m_world = new World(world_info);
        /*
        m_tile_material = make_tile_material<m_map_config.num_tiles_x * m_map_config.num_tiles_y>(m_material_manager, m_graphics_context.get_shader_manager(), m_shader_id);
        m_entity_manager.get_registry().ctx().emplace<ecs::Tile::SMaterialID>(m_tile_material);
        //ecs::Tile::g_tile_material_id = m_tile_material;
        m_world_tiles = world_generator::generateGround(m_entity_manager, {m_map_config.num_tiles_x, m_map_config.num_tiles_y}, {0,0});//todo
        */
        // m_buffer = makeSimpleRenderBuffer<m_map_config.num_tiles_x * m_map_config.num_tiles_y>();
        
        CHECK_GL_ERROR();
        ecs::MovementKeys keys =  ecs::MovementKeys{
            .up = GLFW_KEY_W,
            .down = GLFW_KEY_S,
            .left = GLFW_KEY_A,
            .right = GLFW_KEY_S,
        };
        Syris::texture::Texture2DBundle texture = ecs::Player::defaultTextureBundle();
        //m_player_id = ecs::Player::newPlayer({0,0}, keys, texture, m_entity_manager.get_registry());

        /*
        uint32_t player_shader_id = m_graphics_context.get_shader_manager().add_shader(player_shader::get_shader_info());
        std::cout << "shader id player: " << player_shader_id << '\n';
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }*/
        m_entity_material_id = make_entity_material(m_entity_manager, m_graphics_context.get_shader_manager(), m_statistics);
        m_entity_manager.get_registry().ctx().emplace<ecs::SMaterialID>(m_entity_material_id);
        m_entity_manager.get_registry().ctx().emplace<ecs::CollectableManager>();
        //ecs::Tile::g_entity_material_id = m_entity_material_id;
        m_player_id = ecs::Player::newPlayerEntity({0,0}, keys, texture, m_entity_manager, m_entity_material_id);
        
        //update_data(false);
        Syris::Logger::client_info("sandbox scene successfully created"); 
        hpx::async(std::bind(&SandboxScene::sim_loop, this));
    }

    void SandboxScene::sim_loop(){
        m_sim_fps.start();
        AsyncToSyncQueue::AsyncFunction func{
            .function = std::bind(&Syris::engine_time::FPS::render_frame_count, &m_sim_fps),
            .calls_to_be_consumed = std::nullopt,
        };
        //m_async_to_sync_queue.add(func);
        constexpr bool single_thread = false;
        std::size_t num_threads = 3;
        // Create a dynamic chunk size with calculated chunk size
        while(m_sim_loop_running){
            auto tick_group = m_entity_manager.get_registry().group<ecs::CTickFast>();
            m_sim_fps.next_frame();
            float delta_ms = m_sim_fps.get_time().get_delta_ms(); 
            auto [cPos, cMovenmentSpeed, cSpeed] = m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>, ecs::CMovementSpeed, ecs::CSpeed>(m_player_id);
            cPos.set({cPos.get().pos + cSpeed.speed * cMovenmentSpeed.movement_speed * delta_ms / 1000.f});
            //std::cout << "Player: pos[" << cPos.get().pos << "] dir[" << cDir.value << "] speed[" << cSpeed.value << "] with " <<  delta_ms <<"ms\n";
            if constexpr (single_thread){
                for (auto entity : tick_group)
                {
                    // m_entity_manager.get_registry().get<ecs::CTickFast>(entity).tick(m_entity_manager, entity, m_sim_fps.get_time());
                    tick_group.get<ecs::CTickFast>(entity).tick(m_entity_manager, entity, m_sim_fps.get_time());
                }
            }else{
                /*
                std::size_t num_entities = tick_group.size();
                hpx::for_loop(hpx::execution::par, 0, num_entities, [&](std::size_t i)
                              { tick_group.get<ecs::CTickFast>(tick_group[i]).tick(m_entity_manager, tick_group[i], m_sim_fps.get_time());
                });
                */
                std::size_t num_entities = tick_group.size();
                std::size_t chunk_size_value = num_entities + num_threads - 1 / num_threads;
                hpx::execution::experimental::dynamic_chunk_size chunk_size(chunk_size_value);
                auto policy = hpx::execution::par.with(chunk_size);

                hpx::for_loop(hpx::execution::par, 0, num_threads,
                    [&](std::size_t chunk_index) {
                        std::size_t start = chunk_index * chunk_size_value;
                        std::size_t end = std::min(start + chunk_size_value, num_entities);

                        for (std::size_t i = start; i < end; ++i) {
                            if (i < tick_group.size())
                                tick_group.get<ecs::CTickFast>(tick_group[i]).tick(m_entity_manager, tick_group[i], m_sim_fps.get_time());
                        }
                    }
                );
            }

        }
    }

    void SandboxScene::update_data(bool imgui){/*
        //using material
        auto render_group = this->m_entity_manager.get_registry().group<>(entt::get<ecs::CTexture,
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
                .mem_offset = index * sizeof(TileInstancedData),
                .size = sizeof(TileInstancedData),
                .data = &data
            };
            m_material->set_data(data_set);
            ///glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(TileInstancedData), &data);
            CHECK_GL_ERROR();

            index++;
        }*/
    }
    void SandboxScene::on_update(Syris::engine_time::Time& time){
        //m_async_to_sync_queue.exec_all(); todo
        
        
        //move camera
        //m_camera.on_update(time);

        //ecs::Player::animate(m_registry, m_player_id, time);

        //render shader hot reload

        //draw tiles
        m_world->draw({ m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture() });
       /* sandbox_scene::ShaderLayoutTuple data = ;
        m_material_manager.draw(m_tile_material, &data);
        */
   
        //player camera entities

        ecs::Player::sync(m_entity_manager, m_material_manager, m_entity_material_id, m_player_id);

        ecs::AsyncComponent<ecs::CPosition>& cPos =  m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>>(m_player_id);
        ecs::CSpeed& cSpeed = m_entity_manager.get_registry().get<ecs::CSpeed>(m_player_id);
        m_camera.getCamera().set_position({ cPos.get().pos, 0.f });
        ImGui::Begin("Player pos");
        ImGui::Text("Player pos {%.1f,%.1f}", cPos.get().pos.x, cPos.get().pos.y);
        ImGui::Text("Player speed {%.1f,%.1f}", cSpeed.speed.x, cSpeed.speed.y);
        ImGui::End();
        entity_shader::ShaderLayoutTuple data_player = {m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture()};
        m_material_manager.get_material(m_entity_material_id)->draw(&data_player);
        ImGui::Begin("Entity Material");
        ImGui::Text("Entity count %d",  m_material_manager.get_material(m_entity_material_id)->get_entity_count());
        ImGui::Text("Created bushes count %lld",  ecs::Bush::g_number_of_bushes.load());
        ImGui::End();
        m_sim_fps.render_frame_count();
    }

    inline float closest_to(float val) {
        if (val < 0) {
            return -1;
        }if (val > 0) {
            return 1;
        }
        return 0;
    }

    inline glm::vec2 closest_to(glm::vec2 val) {
        return { closest_to(val.x), closest_to(val.y) };
    }

    bool SandboxScene::on_event(Syris::Event* event){
        if (event->get_type() == Syris::EventType::KeyPressed){
            Syris::EventKey *event_key = (Syris::EventKey*)event; 
            int key = event_key->get_key();
            
            auto [cDir, cSpeed, cMovementSpeed] = m_entity_manager.get_registry().get<ecs::CDir, ecs::CSpeed, ecs::CMovementSpeed>(m_player_id);
            cSpeed.speed = closest_to(cSpeed.speed);
            float speed_to_aply;
            if (event_key->get_action()== GLFW_PRESS)
                speed_to_aply = 1.f;
            else if (event_key->get_action() == GLFW_RELEASE)
                speed_to_aply = -1.f;
            else {
                //exit(1);
                return true;
            }
            switch (key){
            case GLFW_KEY_W:
                cDir.value.y += speed_to_aply;
                //cDir.value = glm::normalize(cDir.value);

                cSpeed.speed.y += speed_to_aply;
//                cSpeed.speed = glm::normalize(cSpeed.speed);
                break;
            case GLFW_KEY_S:
                cDir.value.y -= speed_to_aply;
  //              cDir.value = glm::normalize(cDir.value);

                cSpeed.speed.y -= speed_to_aply;
//                cSpeed.speed = glm::normalize(cSpeed.speed);
                // cDir.value = {0.f,-1.f};
                break;
            case GLFW_KEY_A:
                cDir.value.x -= speed_to_aply;
//                cDir.value = glm::normalize(cDir.value);

                cSpeed.speed.x -= speed_to_aply;
//                cSpeed.speed = glm::normalize(cSpeed.speed);
                // cDir.value = {-1.f,0.f};
                break;
            case GLFW_KEY_D:
                cDir.value.x += speed_to_aply;
  //              cDir.value = glm::normalize(cDir.value);

                cSpeed.speed.x += speed_to_aply;
//                cSpeed.speed = glm::normalize(cSpeed.speed);
                // cDir.value = {1.f,0.f};
                break;
            default:
                return true;
            }
            if (glm::abs(cSpeed.speed.x) + glm::abs(cSpeed.speed.y) == 0)
                cSpeed.speed = {0,0};
            else
                cSpeed.speed = glm::normalize(cSpeed.speed);
            if (cDir.value.x + cDir.value.y == 0)
                cDir.value = {0,0};
            else
                cDir.value = glm::normalize(cDir.value);
        }
        //Syris::Logger::client_info(std::format("event type {}", (int)event->get_type()).c_str());
        return false;
    }
    SandboxScene::~SandboxScene(){
        m_sim_loop_running = false;
    }
}

