#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"
#include "Syris/texture/Texture.hpp"
#include "util.h"

namespace Sandbox::fragment_tile_rendering{
    /*const char* path = "fragment_tile_rendering";
    
    using ShaderLayout = Syris::ShaderLayout<glm::mat4>;
    using ShaderLayoutTuple = std::tuple<glm::mat4>;
    static Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "fragment_tile_rendering";
        std::vector<std::string> names = {"ViewProjection"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
    static Syris::BatchRenderer::CreateInfo get_tile_material(Syris::ShaderManager& shader_manager, RawData& vertex_buffer_rd, RawData& instance_buffer_rd, Syris::IndexBuffer::CreateInfo& index_buffer_info, uint32_t instances, Syris::Statistics& statistics){
        Syris::ShaderManager::ShaderID shader_id = shader_manager.add_shader(get_shader_info());
        if (shader_id == 0){
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

        Syris::AttributeLayoutList vertex_attribute_list({vertex_layouts_info.begin(), vertex_layouts_info.end()}, 0);


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
        AttCreateInfo instanced_material{
            .values_count = 3,
            .value_type = Syris::ValueType::Float,
            .normalize = false,
            .perInstance = true
        };

        std::array<AttCreateInfo, 5> instanced_layouts_info = {instanced_model_col_0, instanced_model_col_1, instanced_model_col_2, instanced_model_col_3, instanced_material};
        Syris::AttributeLayoutList instance_attribute_list({instanced_layouts_info.begin(), instanced_layouts_info.end()}, vertex_attribute_list.attribute_size());


        //set the data to the buffers
        using BufferInfo = Syris::VertexBuffer::SubBufferInfo;
        BufferInfo vertex_buffer = BufferInfo{
            .layout_list = vertex_attribute_list,
            .size = vertex_buffer_rd.size,
            .data = vertex_buffer_rd.data
        };
        BufferInfo instance_buffer{
            .layout_list = instance_attribute_list,
            .size = instance_buffer_rd.size,
            .data = instance_buffer_rd.data,
        };

        std::array<BufferInfo, 2> buffers = {vertex_buffer, instance_buffer};
        Syris::VertexBuffer::CreateInfo vertex_buffer_info = Syris::VertexBuffer::CreateInfo{
            .dynamic = false,
            .buffers_info = {buffers.begin(), buffers.end()},
            .statistics = statistics
        };
        return Syris::BatchRenderer::CreateInfo{
            .name = "world tiles",
            .shader_manager = shader_manager,
            .shader_id = shader_id,
            .vertex_buffer_info = vertex_buffer_info,
            .index_buffer_info = index_buffer_info,
            .instance_count = 0,//size
            .statistics = statistics
        };
    }
    */
}