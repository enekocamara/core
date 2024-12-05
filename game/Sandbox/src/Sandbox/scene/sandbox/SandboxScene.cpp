#include "SandboxScene.hpp"

#include <array>
#include <format>
#include <vector>
#include <future>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/utils/glm_stdout.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/statistics/Components.hpp"

#include "Sandbox/shaders/entity_shader.h"
#include "Sandbox/ecs/Components.h"
#include "Sandbox/ecs/Tile.h"
#include "Sandbox/ecs/Bush.h"
#include "Sandbox/ecs/Player.h"
#include "Sandbox/scene/helper.h"
#include "Sandbox/ecs/Chicken.h"

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
        m_thread_pool(info.thread_pool),
        m_graphics_context(info.graphics_context),
        m_dll(info.dll),
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
                
        CHECK_GL_ERROR();
        
        Syris::Texture2DBundle texture = ecs::Player::defaultTextureBundle();

        auto shader_res = m_shader_manager.add_shader({"entity"});
        if (!shader_res)
            throw std::runtime_error(std::format("Failed to add entity shader: {}", shader_res.error()));
        m_shader_id = shader_res.value(); 

        make_entity_renderer();
        m_entity_manager.get_registry().ctx().emplace<ecs::SER_ID>(m_entity_renderer_id);
        m_entity_manager.get_registry().ctx().emplace<ecs::CollectableManager>();
        std::cout << "scene entity renderer id: " << (int)m_entity_renderer_id << '\n';
        World::CreateInfo world_info = World::CreateInfo{
            .m_material_manager = m_material_manager,
            .m_shader_manager = m_shader_manager,
            .m_entity_manager = m_entity_manager,
            .chunks = {1,1},
            .chunk_size = {10,10},
            .tile_size = {1,1},
            .statistics = info.statistics,
            .entity_renderer_id = m_entity_renderer_id,
            .seed = 0
        };
        m_world = new World(world_info);
        ecs::MovementKeys keys =  ecs::MovementKeys{
            .up = GLFW_KEY_W,
            .down = GLFW_KEY_S,
            .left = GLFW_KEY_A,
            .right = GLFW_KEY_S,
        };
        m_player_id = ecs::Player::newPlayerEntity({0,0}, keys, texture, m_entity_manager, m_entity_renderer_id);
        ecs::Chicken::newChickenEntity({0,0}, m_entity_manager, m_entity_renderer_id, m_dll);
        Syris::Logger::client_info("sandbox scene successfully created");
        //hpx::async(std::bind(&SandboxScene::sim_loop, this));
        m_thread_pool.enqueue(std::bind(&SandboxScene::sim_loop, this));
    }

    void SandboxScene::sim_loop(){
        m_sim_fps.start();
        /*
        AsyncToSyncQueue::AsyncFunction func{
            .function = std::bind(&Syris::engine_time::FPS::render_frame_count, &m_sim_fps),
            .calls_to_be_consumed = std::nullopt,
        };*/
        
        std::vector<std::future<void>> futures;
        while(m_sim_loop_running){
            m_sim_fps.next_frame();

            m_entity_manager.lock_active_mutex();
            auto tick_group = m_entity_manager.get_registry().group<ecs::CTickFast>();


            const uint32_t num_entities = tick_group.size();
            const uint32_t chunk_size_value = (num_entities + m_sim_thread_count - 1) / m_sim_thread_count;
            futures.reserve(m_sim_thread_count);

            for (uint32_t i = 0; i < m_sim_thread_count; i++)
            {
                futures.push_back(m_thread_pool.enqueue([tick_group, i, chunk_size_value, num_entities, this](){
                        uint32_t start = i * chunk_size_value;
                        uint32_t end = std::min(start + chunk_size_value, num_entities);
                        for (uint32_t entt_index = start; entt_index < end; entt_index++){
                            entt::entity entity = tick_group[entt_index];
                            //if (m_entity_manager.get_registry().all_of<Syris::ecs::Active>(entity))
                                tick_group.get<ecs::CTickFast>(entity).tick(m_entity_manager, entity, m_sim_fps.get_time());
                        } 
                    }));
            }
            for (auto &future : futures){
                future.get(); // Wait for each task to complete
            }
            auto& op_chunk_system = m_entity_manager.get_chunk_system();
            if (op_chunk_system)
                op_chunk_system->sync();
            futures.clear();
            m_entity_manager.unlock_active_mutex();
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
        //auto [cPos, cMovenmentSpeed, cSpeed] = m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>, ecs::CMovementSpeed, ecs::CSpeed>(m_player_id);
        
        ecs::Player::sync(m_entity_manager, m_material_manager, m_entity_renderer_id, m_player_id);
        if (m_render_window_size != m_graphics_context.get_current_render_window_size()){
            m_render_window_size = m_graphics_context.get_current_render_window_size();
            m_camera = Syris::OrthographicCameraController({m_render_window_size.x / (float)m_render_window_size.y, 10.f});
        }
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
        camera.pnext = nullptr;
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

