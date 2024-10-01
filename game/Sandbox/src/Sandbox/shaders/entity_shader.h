#pragma once
#include "Syris/shader/ShaderManger.hpp"
#include "../scene/helper.h"
#include "Syris/materials/Material.hpp"
#include "Syris/renderer/AttributeLayoutList.hpp"
#include "Syris/log/Log.h"

namespace Sandbox::entity_shader{

    struct RawData{
        void *data;
        uint64_t size;
    };

    using ShaderLayout = Syris::ShaderLayout<glm::mat4, Syris::texture::Texture2D>;
    using ShaderLayoutTuple = std::tuple<glm::mat4, Syris::texture::Texture2D>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "entity";
        std::vector<std::string> names = {"ViewProjection", "texture1"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }

    Syris::Material* make_entity_material(Syris::ShaderManager& shader_manager, RawData& vertex_buffer_rd, RawData& instance_buffer_rd, Syris::IndexBuffer::CreateInfo& index_buffer_info, uint32_t instances){
        Syris::ShaderManager::ShaderID shader_id = shader_manager.add_shader(get_shader_info());
        if (shader_id == 0){
            CLIENT_ERROR("failed to make instance material");
            return nullptr;
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
            .size = vertex_buffer_rd.size,//sizeof(TileVertices) * size,
            .data = vertex_buffer_rd.data,
        };
        BufferInfo instance_buffer{
            .layout_list = instance_attribute_list,
            .size = instance_buffer_rd.size, //sizeof(TileInstancedData) * size,
            .data = instance_buffer_rd.data//instanced_data.data()
        };

        std::array<BufferInfo, 2> buffers = {vertex_buffer, instance_buffer};
        Syris::VertexBuffer::CreateInfo vertex_buffer_info = Syris::VertexBuffer::CreateInfo{
            .dynamic = false,
            .buffers_info = {buffers.begin(), buffers.end()},
        };

        /*const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl";
        const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl";
        */
        
        Syris::Material::CreateInfo material_create_info = Syris::Material::CreateInfo{
            .shader_manager = shader_manager,
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = instances,
        };
        return Syris::Material::create_material(material_create_info);
    }
}