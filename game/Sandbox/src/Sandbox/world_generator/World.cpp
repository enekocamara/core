#include <glm/glm.hpp>
#include <FastNoiseLite.h>
#include <random> 
#include "World.hpp"
#include "Sandbox/ecs/Components.h"
#include "TileMaterial.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererLayout.hpp"
#include "Syris/types/Type.h"
#include "Sandbox/scene/helper.h"
#include "Sandbox/ecs/Tile.h"

namespace Sandbox{
    void World::make_tile_renderer(){
        TileVertices vertices = TileVertices();
        TileIndices indices = TileIndices();

        Syris::IndexBuffer::CreateInfo index_buffer_info = Syris::IndexBuffer::CreateInfo{
            .indices_count = indices.vertices.size(),
            .indices = indices.vertices.data(),
            .dynamic = false,
        };

        std::unique_ptr<Syris::BatchRendererLayout> layout = std::make_unique<Syris::BatchRendererLayout>(m_shader_manager.get_shader(m_shader_id), m_statistics);
        Syris::VertexBuffer::SubBufferInfo pos(false, false);
        pos.push({"aPos", Syris::Type::vec2});
        
        auto pos_res = layout->set_subbuffer(pos);
        if (!pos_res)
            BREAK_POINT(std::format("failed to set pos laytout: {}", pos_res.error()));
        /*auto instance_res = layout->set_subbuffer(instance);
        if (!instance_res)
            BREAK_POINT(std::format("failed to set instance laytout: {}", instance_res.error()));*/
            
        auto layout_res = layout->finish();
        if (!layout_res){
            throw std::runtime_error(std::format("not all layout attributes where set: {}", layout_res.error()));
        }
        Syris::BatchRenderer::CreateInfo info{
            .name = "world tiles",
            .shader_manager = m_shader_manager,
            .shader_id = m_shader_id,
            .index_buffer_info = index_buffer_info,
            .layout = std::move(layout),
            .statistics = m_statistics,
        };
        m_tile_renderer_id = m_renderer_manager.add_renderer(info);
        Syris::BR_SetAttributeRequest positions{
            .values = {vertices.vertices.begin(), vertices.vertices.end()}
        };
        m_renderer_manager.get_renderer(m_tile_renderer_id)->set_attribute(positions);
    }

    World::World(CreateInfo info):
        m_seed(info.seed),
        m_entity_manager(info.m_entity_manager),
        m_renderer_manager(info.m_material_manager),
        m_shader_manager(info.m_shader_manager),
        m_entity_renderer_id(info.entity_renderer_id),
        m_world_dimmensions(info.chunk_size * info.chunks),
        m_statistics(info.statistics) {
        
        m_entity_manager.create_chunk_system(info.chunks, info.chunk_size, info.tile_size);

        Syris::Shader::CreateInfo shader_info{
            .path = "fragment_tile_rendering_v2"
        };
        auto shader_id_res = m_shader_manager.add_shader(shader_info);
        if (!shader_id_res){
            throw std::runtime_error(std::format("Failed to add shader 'fragment tile rendering': {}", shader_id_res.error()));
        }
        m_shader_id = shader_id_res.value();
        auto text_shader_res = m_shader_manager.add_shader({"texture_display"});
        if (!shader_id_res){
            throw std::runtime_error(std::format("Failed to add shader 'fragment tile rendering': {}", shader_id_res.error()));
        }
        m_texture_shader = text_shader_res.value();

        //m_storage.get_storage<ecs::CTileData>().resize(m_world_dimmensions.x * m_world_dimmensions.y);

        make_tile_renderer();
        m_entity_manager.get_registry().ctx().emplace<ecs::Tile::SER_ID>(m_tile_renderer_id);
        m_entity_manager.get_registry().ctx().emplace<World&>(*this);
        create_world();
    }

    World::~World(){
        delete m_world_tiles;
    }

    void World::create_world(){
        std::vector<entt::entity> data(m_world_dimmensions.x * m_world_dimmensions.y);
        Syris::NoisePipeline<glm::vec3> pipeline(m_noise_pattern);
        Syris::NoiseGenerator<glm::vec3> noise_generator(pipeline);
        for (std::size_t y = 0; y < m_world_dimmensions.y; y++){
            for(std::size_t x = 0; x < m_world_dimmensions.x; x++){
                glm::vec3 noise = noise_generator.generate_noise({ x,y });
                data[idx({x,y})] = ecs::Tile::new_tile(m_entity_manager, y * m_world_dimmensions.x + x, 0, {x, y}, *(ecs::CTileData*)&noise);
            }
        }
        Syris::GridLookUp<entt::entity>::CreateInfo info{
            .grid_dimmension = m_world_dimmensions,
            .data = std::move(data) 
        };
        m_world_tiles = new Syris::GridLookUp<entt::entity>(std::move(info));
        create_textures();
    }
    void World::create_textures(){
        /*Syris::FrameTexture2D::CreateInfo temp_frame_text_info{
            .texture = m_temp_text.get(),
            .shader_manager = m_shader_manager,
            .shader_id = m_texture_shader,
            .size = m_world_dimmensions,
        };
        m_temp_frame_text = std::make_unique<Syris::FrameTexture2D>(temp_frame_text_info);

        Syris::FrameTexture2D::CreateInfo height_frame_text_info{
            .texture = m_height_text.get(),
            .shader_manager = m_shader_manager,
            .shader_id = m_texture_shader,
            .size = m_world_dimmensions,
        };
        m_height_frame_text = std::make_unique<Syris::FrameTexture2D>(height_frame_text_info);

        Syris::FrameTexture2D::CreateInfo humd_frame_text_info{
            .texture = m_humd_text.get(),
            .shader_manager = m_shader_manager,
            .shader_id = m_texture_shader,
            .size = m_world_dimmensions,
        };
        m_humd_frame_text = std::make_unique<Syris::FrameTexture2D>(humd_frame_text_info);
        
        m_height_texture_editor = std::make_unique<Syris::TextureEditor>(*m_height_frame_text.get(), "height editor");
        */
        static std::vector<ecs::CTileData> data(m_world_dimmensions.x * m_world_dimmensions.y);
    
        for (std::size_t y = 0; y < m_world_dimmensions.y; y++){
            for(std::size_t x = 0; x < m_world_dimmensions.x; x++){
                data[idx({x,y})] = m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CTileData>>(m_world_tiles->get({x,y})).get();
            }
        }
        Syris::Texture2DCreateBundle bundle{
            .internal_format = GL_RGB,
                .image_format = GL_RGB,
                .wrap_s = GL_REPEAT,
                .wrap_t = GL_REPEAT,
                .filter_min = GL_NEAREST,
                .filter_max = GL_NEAREST
        };
        m_combined_text = std::make_unique<Syris::Texture2D>(*(std::vector<glm::vec3>*)&data,m_world_dimmensions, bundle);

        Syris::FrameTexture2D::CreateInfo comb_frame_text_info{
            .texture = m_combined_text.get(),
            .shader_manager = m_shader_manager,
            .shader_id = m_texture_shader,
            .size = m_world_dimmensions,
        };
        m_combined_frame_text = std::make_unique<Syris::FrameTexture2D>(comb_frame_text_info); 
        m_combined_texture_editor = std::make_unique<Syris::TextureEditor>(*m_combined_frame_text.get(), "comb"); 
    }
    /*
            m_entity_manager.exec_func([&](entt::registry& registry){
                std::cout << "Destroying all collectables\n";
                std::vector<entt::entity> entities_to_destroy;
                auto view = registry.view<ecs::CCollectable>();
                entities_to_destroy.reserve(view.size());
                for(entt::entity entity : view){
                    entities_to_destroy.push_back(entity);
                }
                auto renderer = m_renderer_manager.get_renderer(m_entity_renderer_id);
                for(entt::entity entity : entities_to_destroy){
                    //m_entity_manager.delete_entity(entity);
                    Syris::BR_RemoveRequest request{entity};
                    renderer->remove_entity({request});
                    registry.destroy(entity);
                }
            });
            //destroy all consumibles
        }
        auto [noise, warp] = m_noise_pattern.get_noise(); 
        Syris::NoiseGenerator<glm::vec3> noise_generator(Syris::NoisePipeline{m_noise_pattern});
        //auto& mem = m_storage.get_storage<ecs::CTileData>();
        auto apply_noise = [this](glm::vec3 noise_values, glm::uvec2 pos){
            m_entity_manager.new_entity(); m_world_tiles->get_entity(pos) mem[pos.y * m_world_dimmensions.x + pos.x] = *(ecs::CTileData*)(&noise_values);
        };
        noise_generator.generate_noise({0,0}, m_world_dimmensions, apply_noise);
        Syris::Texture2DCreateBundle bundle{
            .internal_format = GL_RGB,
                .image_format = GL_RGB,
                .wrap_s = GL_REPEAT,
                .wrap_t = GL_REPEAT,
                .filter_min = GL_LINEAR,
                .filter_max = GL_LINEAR
        };
    }*/

    void World::draw(Syris::Uniform* uniforms){
        static bool first = true;
        
        if (first){
            m_combined_frame_text->render();

            //m_temp_frame_text->render();
            //m_height_frame_text->render();
            //m_humd_frame_text->render();
            first = false;
        }

        ImGui::Begin("Texture Display");
        ImGui::Text("Current seed %d", m_seed);ImGui::SameLine();
        /*if (ImGui::Button("Recreate world")){
            m_seed = std::rand();
            create_world(true);
            m_temp_frame_text->render();
            m_height_frame_text->render();
            m_humd_frame_text->render();
        }
        if (ImGui::Button("Update height")){
            m_height_text->bind();
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, m_height.data());
            for (std::size_t i = 0; i < m_humidity.size(); i++){
                combined_noise[i] = {m_height[i], m_humidity[i], m_temperature[i]};
            }
            m_combined_text->update(m_storage.get_storage<ecs::CTileData>());
        }*/
       
        ImGui::Text("Combined map");
        ImGui::Image((void *)(intptr_t)m_combined_frame_text->get_texture_id(), ImVec2(400, 400));
        
        /*
        ImGui::Text("Temperature map");
        ImGui::Image((void *)(intptr_t)m_temp_frame_text->get_texture_id(), ImVec2(400, 400));
        ImGui::Text("Humidity map");
        ImGui::Image((void *)(intptr_t)m_humd_frame_text->get_texture_id(), ImVec2(400, 400));
        ImGui::Text("Height map");
        ImGui::Image((void *)(intptr_t)m_height_frame_text->get_texture_id(), ImVec2(400, 400));
*/
        if (m_combined_texture_editor->render()){
            
            m_entity_manager.exec_func([&](entt::registry& registry){
                std::cout << "Destroying all collectables\n";
                std::vector<entt::entity> entities_to_destroy;
                auto view = registry.view<ecs::CCollectable>();
                entities_to_destroy.reserve(view.size());
                for(entt::entity entity : view){
                    entities_to_destroy.push_back(entity);
                }
                auto renderer = m_renderer_manager.get_renderer(m_entity_renderer_id);
                for(entt::entity entity : entities_to_destroy){
                    Syris::BR_RemoveRequest request{entity};
                    renderer->remove_entity({request});
                    auto pos = registry.try_get<Syris::ecs::ChunkedPosition>(entity);
                    if (pos != nullptr) {
                        if (m_entity_manager.get_chunk_system())
                            m_entity_manager.get_chunk_system()->remove_entity(entity, pos->pos);
                        else
                            BREAK_POINT("chunk system must be set");
                    }
                    registry.destroy(entity);
                }
                
            });
            
        } 
        ImGui::End();
        

        Syris::Uniform world_dimmensions{
            .name = "uWorldDimmensions",
            .data = &m_world_dimmensions,
            .pnext = nullptr
        };
        Syris::Uniform combined_text{
            .name = "uTexture",
            .data = m_combined_text.get(),
            .pnext = &world_dimmensions
        };
        uniforms->pnext = &combined_text;
        
        m_renderer_manager.draw(m_tile_renderer_id, uniforms);
    }
    
    void World::update_ground_tiles(){
        auto renderer = m_entity_manager.get_batch_renderer_manager().get_renderer(m_tile_renderer_id);
        for(uint32_t i = 0; i < m_world_dimmensions.x * m_world_dimmensions.y; i++){
            entt::entity entity = m_world_tiles->get(i);
            auto& data = m_entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CTileData>>(entity);
            data.set(m_entity_manager.get_registry().get<ecs::CTileData>(entity));
        }
    }
}