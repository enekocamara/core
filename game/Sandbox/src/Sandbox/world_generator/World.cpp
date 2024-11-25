#include <glm/glm.hpp>
#include <FastNoiseLite.h>
#include "World.hpp"
#include "Sandbox/world_generator/WorldGenerator.hpp"
#include "Sandbox/ecs/Components.h"
#include "TileMaterial.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererLayout.hpp"
#include "Syris/types/Type.h"

namespace Sandbox{
    void World::make_tile_renderer(){
        TileVertices vertices = TileVertices();
        TileIndices indices = TileIndices();

        Syris::IndexBuffer::CreateInfo index_buffer_info = Syris::IndexBuffer::CreateInfo{
            .indices_count = indices.vertices.size(),
            .indices = indices.vertices.data(),
            .dynamic = false,
        };
        /*
        TileVertices tile_vertices = TileVertices();
        RawData vertex_buffer_rd{
            .data = &tile_vertices,
            .size = sizeof(TileVertices),
        };

        RawData instance_buffer_rd{
            .data = nullptr,
            .size = 0,
        };

        Syris::BatchRenderer::CreateInfo info = fragment_tile_rendering::get_tile_material(m_shader_manager, vertex_buffer_rd, instance_buffer_rd, index_buffer_info, m_world_dimmensions.x * m_world_dimmensions.y, m_statistics);
        m_tile_material = m_material_manager.add_material<TileInstancedData>(info);
        */
        std::unique_ptr<Syris::BatchRendererLayout> layout = std::make_unique<Syris::BatchRendererLayout>(m_shader_manager.get_shader(m_shader_id), m_statistics);
        Syris::VertexBuffer::SubBufferInfo pos(false, false);
        pos.push({"aPos", Syris::Type::vec2});
        Syris::VertexBuffer::SubBufferInfo instance(true, true);
        instance.push({"aModel", Syris::Type::mat4});
        instance.push({"aMaterial", Syris::Type::vec3});
        layout->set_subbuffer(pos);
        layout->set_subbuffer(instance);
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
        m_tile_renderer_id = m_renderer_manager.add_renderer<TileInstancedData>(info);
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
        m_world_dimmensions(info.world_dimmensions),
        m_statistics(info.statistics) {
        
        Syris::Shader::CreateInfo shader_info{
            .path = "fragment_tile_rendering"
        };
        auto shader_id_res = m_shader_manager.add_shader(shader_info);
        if (!shader_id_res){
            throw std::runtime_error(std::format("Failed to add shader 'fragment tile rendering': {}", shader_id_res.error()));
        }
        m_shader_id = shader_id_res.value();
        m_humidity.resize(m_world_dimmensions.x * m_world_dimmensions.y);
        m_temperature.resize(m_world_dimmensions.x * m_world_dimmensions.y);
        m_height.resize(m_world_dimmensions.x * m_world_dimmensions.y);
        gen_2d_noise_vector(m_humidity, m_world_dimmensions, m_seed + 2);
        gen_2d_noise_vector(m_temperature, m_world_dimmensions, m_seed + 3);
        gen_2d_noise_vector(m_height, m_world_dimmensions, m_seed + 4);
        m_height_text = std::make_unique<Syris::Texture2D>(m_height, m_world_dimmensions);
        m_temp_text = std::make_unique<Syris::Texture2D>(m_temperature, m_world_dimmensions);
        m_humd_text = std::make_unique<Syris::Texture2D>(m_humidity, m_world_dimmensions);

        auto text_shader_res = m_shader_manager.add_shader({"texture_display"});
        if (!shader_id_res){
            throw std::runtime_error(std::format("Failed to add shader 'fragment tile rendering': {}", shader_id_res.error()));
        }
        m_texture_shader = text_shader_res.value();

        Syris::FrameTexture2D::CreateInfo temp_frame_text_info{
            .texture = m_height_text.get(),
            .shader_manager = m_shader_manager,
            .shader_id = m_texture_shader,
            .size = m_world_dimmensions,
        };
        m_temp_frame_text = std::make_unique<Syris::FrameTexture2D>(temp_frame_text_info);
        make_tile_renderer();
        m_entity_manager.get_registry().ctx().emplace<ecs::Tile::SER_ID>(m_tile_renderer_id);
        generate_ground();
    }

    World::~World(){
        delete m_world_tiles;
    }

    void World::draw(Syris::Uniform* uniforms){
        m_height_text->bind();
        // Read the texture data into a buffer
        std::vector<GLfloat> pixels(m_world_dimmensions.x * m_world_dimmensions.y); // Allocate buffer for the data

        // Read the texture data (assuming it's single-channel float, like GL_R32F)
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, pixels.data());

        m_temp_frame_text->render();
        ImGui::Begin("Texture Display");
        ImGui::Image((void *)(intptr_t)m_temp_frame_text->get_texture_id(), ImVec2(m_world_dimmensions.x, m_world_dimmensions.y));
        ImGui::End();
        /*
        if (ImGui::BeginTable("World Tiles", m_world_dimmensions.x)) // 3 columns
        {
            for (int y = m_world_dimmensions.y - 1; y >= 0; --y) // 5 rows
            {
                ImGui::TableNextRow();
                for (int x = 0; x < m_world_dimmensions.x; ++x)
                {
                    ImGui::TableNextColumn();
                    entt::entity tile = m_world_tiles->get_entity(y * m_world_dimmensions.x + x); 
                    //ImGui::Text("%.1f,%.1f", m_entity_manager.get_registry().get<ecs::CPosition>(tile).pos.x, m_entity_manager.get_registry().get<ecs::CPosition>(tile).pos.y);
                    ImGui::Text("%.3f", m_temperature[idx({x,y})]);
                }
            }

            ImGui::EndTable();
        }*/
        m_renderer_manager.draw(m_tile_renderer_id, uniforms);
    }
    void World::gen_2d_noise_vector(std::vector<float> &texture, glm::uvec2 dimmensions, int seed){
        FastNoiseLite noise;
        noise.SetSeed(seed);
        noise.SetFractalOctaves(5);
        noise.SetFractalLacunarity(1);
        noise.SetFrequency(0.1);
        noise.SetFractalGain(0.5f);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        for (uint32_t j = 0; j < dimmensions.y; j++){
            for (uint32_t i = 0; i < dimmensions.x; i++){
                texture[j * dimmensions.x + i] = noise.GetNoise((float)i, (float)j);
            }
        }
    }
    entt::entity World::gen_ground_tile(glm::uvec2 pos, Syris::EntityManager& entity_manager, uint32_t index){
        TileInstancedData data;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pos, 0.0f));
        data.trans = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
        data.mat = TileInstanceMaterial{m_height[idx(pos)], m_humidity[idx(pos)], m_temperature[idx(pos)]};
        return ecs::Tile::newTile(&data, entity_manager, index, 0, pos);
    }
    void World::generate_ground(){
        GridLookUp::CreateInfo info{
            .grid_dimmension = m_world_dimmensions,
            .tile_dimmensions = {0,0},//todo
            .tile_gen = std::bind(&World::gen_ground_tile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            .entity_manager = m_entity_manager
        };
        m_world_tiles = new GridLookUp(info);
    }
}