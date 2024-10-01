#pragma once
#include <optional>

#include "Material.hpp"
#include "Syris/shader/OpenGLShader.hpp"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLIndexBuffer.hpp"
//i want to combine updating data with shaders
//render buffer is a implementation of a system i want to make

namespace Syris{
    class OpenGLMaterial : public Material{
        public:
            OpenGLMaterial(CreateInfo& info);
            ~OpenGLMaterial();
            void set_data(DataSet& data)override;
            void draw(void* shader_uniforms)override;
        private:
            ShaderManager& m_shader_manager; 
            ShaderManager::ShaderID m_shader_id;
            std::optional<OpenGLIndexBuffer> m_index_Buffer;
            std::optional<uint32_t> m_instance_count;
            OpenGLVertexBuffer m_vertex_buffers;
    };
}