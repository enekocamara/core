#pragma once
#include <span>
#include <optional>

#include "Syris/shader/ShaderManger.hpp"
#include "Syris/renderer/VertexBuffer.hpp"
#include "Syris/renderer/IndexBuffer.hpp"
#include "Syris/renderer/AttributeLayoutList.hpp"
#include "Syris/renderer/Buffer.h"
namespace Syris{
    class Material{
        public:
            struct CreateInfo{
                //Shader::CreateInfo shader_info;
                ShaderManager& shader_manager;
                ShaderManager::ShaderID shader_id;
                VertexBuffer::CreateInfo vertex_buffer_info;
                std::optional<IndexBuffer::CreateInfo> index_buffer_info;
                std::optional<uint32_t> instance_count;
            };
            struct DataSet{
                bool index_buffer;
                std::optional<uint32_t> sub_buffer_index;
                uint32_t index;
                uint32_t size;
                void *data;
            };
            virtual ~Material(){}
            virtual void set_data(DataSet& data) = 0; 
            virtual void draw(void *shader_uniforms) = 0;
            static Material* create_material(CreateInfo& info);
    };
}