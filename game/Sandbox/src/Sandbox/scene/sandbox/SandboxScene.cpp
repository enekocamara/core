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
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/statistics/Components.hpp"

#include "SandboxScene.hpp"
#include "Sandbox/shaders/entity_shader.h"
#include "Sandbox/world_generator/WorldGenerator.hpp"
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
    void SandboxScene::make_entity_renderer(){
        
        TileVertices tile_vertices = TileVertices();
                
        TileIndices tile_indices = TileIndices();

        Syris::IndexBuffer::CreateInfo index_buffer_info{ 
            .indices_count = 6,
            .indices = tile_indices.vertices.data(),
            .dynamic = false
        };

        
        std::unique_ptr<Syris::BatchRendererLayout> layout = std::make_unique<Syris::BatchRendererLayout>(m_shader_manager.get_shader(m_shader_id), m_statistics);
        Syris::VertexBuffer::SubBufferInfo pos(false, false);
        pos.push({"aPos", Syris::Type::vec2});
        Syris::VertexBuffer::SubBufferInfo instance(true, true);
        instance.push({"aTexCoord", Syris::Type::vec4});
        instance.push({"aModel", Syris::Type::mat4});
        layout->set_subbuffer(pos);
        layout->set_subbuffer(instance);
        auto layout_res = layout->finish();
        if (!layout_res){
            throw std::runtime_error(std::format("Not all attributes were set {}", layout_res.error()));
        }

        Syris::BatchRenderer::CreateInfo info{
            .name = "entity renderer",
            .shader_manager = m_shader_manager,
            .shader_id = m_shader_id,
            .index_buffer_info = index_buffer_info,
            .layout = std::move(layout),
            .statistics = m_statistics
        };
        
        m_entity_renderer_id  = m_material_manager.add_renderer<QuadTexInstancedData>(info);
        Syris::BR_SetAttributeRequest positions{
            .values = {tile_vertices.vertices.begin(), tile_vertices.vertices.end()}
        };
        m_material_manager.get_renderer(m_entity_renderer_id)->set_attribute(positions);
    }

    SandboxScene::SandboxScene(CreateInfo info) :
        m_material_manager({info.statistics}),
        m_entity_manager({ m_material_manager}),
        m_texture_atlas(info.atlas_path),
        m_shader_manager(info.shader_manager),
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
            .m_shader_manager = m_shader_manager,
            .m_entity_manager = m_entity_manager,
            .world_dimmensions = {200,200},
            .statistics = info.statistics
        };
        m_world = new World(world_info);
                
        CHECK_GL_ERROR();
        
        Syris::Texture2DBundle texture = ecs::Player::defaultTextureBundle();

        auto shader_res = m_shader_manager.add_shader({"entity"});
        if (!shader_res)
            throw std::runtime_error(std::format("Failed to add entity shader: {}", shader_res.error()));
        m_shader_id = shader_res.value(); 

        make_entity_renderer();
        m_entity_manager.get_registry().ctx().emplace<ecs::SER_ID>(m_entity_renderer_id);
        m_entity_manager.get_registry().ctx().emplace<ecs::CollectableManager>();


        ecs::MovementKeys keys =  ecs::MovementKeys{
            .up = GLFW_KEY_W,
            .down = GLFW_KEY_S,
            .left = GLFW_KEY_A,
            .right = GLFW_KEY_S,
        };
        m_player_id = ecs::Player::newPlayerEntity({0,0}, keys, texture, m_entity_manager, m_entity_renderer_id);
        
        Syris::Logger::client_info("sandbox scene successfully created"); 
        //hpx::async(std::bind(&SandboxScene::sim_loop, this));
    }

    void SandboxScene::sim_loop(){
        m_sim_fps.start();
        AsyncToSyncQueue::AsyncFunction func{
            .function = std::bind(&Syris::engine_time::FPS::render_frame_count, &m_sim_fps),
            .calls_to_be_consumed = std::nullopt,
        };
        //m_async_to_sync_queue.add(func);
        constexpr bool single_thread = false;
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
                std::size_t num_entities = tick_group.size();
                std::size_t chunk_size_value = num_entities + m_sim_thread_count - 1 / m_sim_thread_count;
                hpx::execution::experimental::dynamic_chunk_size chunk_size(chunk_size_value);
                auto policy = hpx::execution::par.with(chunk_size);

                hpx::for_loop(hpx::execution::par, 0, m_sim_thread_count,
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
            Syris::BatchRenderer::DataSet data_set{
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
    void SandboxScene::on_update(const Syris::engine_time::Time& time){
        float delta_ms = time.get_delta_ms();
        auto [cPos, cMovenmentSpeed, cSpeed] = m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>, ecs::CMovementSpeed, ecs::CSpeed>(m_player_id);
        cPos.set({cPos.get().pos + cSpeed.speed * cMovenmentSpeed.movement_speed * delta_ms / 1000.f});
        
        ecs::Player::sync(m_entity_manager, m_material_manager, m_entity_renderer_id, m_player_id);

        //ecs::AsyncComponent<ecs::CPosition>& cPos =  m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>>(m_player_id);
        //ecs::CSpeed& cSpeed = m_entity_manager.get_registry().get<ecs::CSpeed>(m_player_id);
        m_camera.getCamera().set_position(glm::vec3(cPos.get().pos, 0.f));

        auto camera_data = m_camera.getCamera().get_view_projection_matrix();

        Syris::Uniform camera{
            .name  = "uViewProjection",
            .data = &camera_data,
            .pnext = nullptr
        };
        m_world->draw(&camera);

        ImGui::Begin("Player pos");
        ImGui::Text("Player pos {%.1f,%.1f}", cPos.get().pos.x, cPos.get().pos.y);
        ImGui::Text("Player speed {%.1f,%.1f}", cSpeed.speed.x, cSpeed.speed.y);
        ImGui::End();

        //entity_shader::ShaderLayoutTuple data_player = {m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture()};
        Syris::Uniform texture{
            .name = "uTexture",
            .data = &m_texture_atlas.getTexture(),
            .pnext = &camera
        };
        
        m_material_manager.get_renderer(m_entity_renderer_id)->draw(&texture);

        ImGui::Begin("Entity Material");
        ImGui::Text("Entity count %d",  m_material_manager.get_renderer(m_entity_renderer_id)->get_entity_count());
        ImGui::Text("Created bushes count %lld",  ecs::Bush::g_number_of_bushes.load());
        ImGui::End();
        ImGui::Begin("Sim Threads");
        ImGui::SliderInt("threads", (int *)&m_sim_thread_count, 1, 12);
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

