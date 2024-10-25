#include <glm/glm.hpp>
#include "World.hpp"
#include "Sandbox/world_generator/world_generator.hpp"
#include "Sandbox/shaders/sandbox_scene_layout.h"
#include "Sandbox/ecs/Components.h"

namespace Sandbox{
    Syris::MaterialManager::MaterialID make_tile_material(Syris::MaterialManager& material_manager, Syris::ShaderManager& shader_manager, Syris::ShaderManager::ShaderID shader_id, Syris::Statistics& statistics){
        //set up default data to fill buffers
        TileVertices vertices = TileVertices();
        TileIndices indices = TileIndices();
        
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
            .size = sizeof(TileVertices),
            .data = &vertices
        };
        BufferInfo instance_buffer{
            .layout_list = instance_attribute_list,
            .size = 0,//sizeof(TileInstancedData) * size,
            .data = nullptr,//instanced_data.data()
        };

        std::array<BufferInfo, 2> buffers = {vertex_buffer, instance_buffer};
        Syris::VertexBuffer::CreateInfo vertex_buffer_info = Syris::VertexBuffer::CreateInfo{
            .dynamic = false,
            .buffers_info = {buffers.begin(), buffers.end()},
            .statistics = statistics
        };

        Syris::IndexBuffer::CreateInfo index_buffer_info = Syris::IndexBuffer::CreateInfo{
            .indices_count = indices.vertices.size(),
            .indices = indices.vertices.data(),
            .dynamic = false,
        };
        Syris::Material::CreateInfo material_create_info = Syris::Material::CreateInfo{
            .name = "world tiles",
            .shader_manager = shader_manager,
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = 0,//size
            .statistics = statistics
        };
        return material_manager.add_material<TileInstancedData>(material_create_info);
    }

    World::World(CreateInfo info):
        m_entity_manager(info.m_entity_manager),
        m_material_manager(info.m_material_manager),
        m_shader_manager(info.m_shader_manager),
        m_world_dimmensions(info.world_dimmensions){
        m_shader_id = m_shader_manager.add_shader(sandbox_scene::get_shader_info());
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }
        m_tile_material = make_tile_material(m_material_manager, m_shader_manager, m_shader_id, info.statistics);
        m_entity_manager.get_registry().ctx().emplace<ecs::Tile::SMaterialID>(m_tile_material);
        m_world_tiles = world_generator::generateGround(m_entity_manager, {m_world_dimmensions.x, m_world_dimmensions.y}, {0,0});//todo
        m_is_init = true;
    }

    World::~World(){
        delete m_world_tiles;
    }

    void World::draw(sandbox_scene::ShaderLayoutTuple data){
        //m_shader_manager.get_shader(m_shader_id)->on_update(time); later
        if (ImGui::BeginTable("World Tiles", m_world_dimmensions.x)) // 3 columns
        {
            for (int y = m_world_dimmensions.y - 1; y >= 0; --y) // 5 rows
            {
                ImGui::TableNextRow();
                for (int x = 0; x < m_world_dimmensions.x; ++x)
                {
                    ImGui::TableNextColumn();
                    entt::entity tile =m_world_tiles->get_entity(y * m_world_dimmensions.x + x); 
                    ImGui::Text("%.3f", m_entity_manager.get_registry().get<ecs::CComposition>(tile).life_matter);
                }
            }

            ImGui::EndTable();
        }
        m_material_manager.draw(m_tile_material, &data);
    }
}