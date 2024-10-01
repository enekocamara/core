#include "OpenGLMaterial.hpp"
#include "Syris/log/Log.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
namespace Syris{
    OpenGLMaterial::OpenGLMaterial(CreateInfo& info):
        m_shader_manager(info.shader_manager),
        m_shader_id(info.shader_id),
        m_vertex_buffers(info.vertex_buffer_info),
        m_instance_count(info.instance_count)
    {
        if (info.index_buffer_info.has_value()){
            m_index_Buffer.emplace(info.index_buffer_info.value());
        }
        if (!m_index_Buffer.has_value() && !m_instance_count.has_value()){
            CORE_ERROR("Render method unsuported");
            exit(1);
        }
    }
    OpenGLMaterial::~OpenGLMaterial(){
        //todo
    }
    void OpenGLMaterial::draw(void *shader_uniforms){
        if (m_index_Buffer.has_value() && m_instance_count.has_value()){
            m_shader_manager.use_shader(m_shader_id, shader_uniforms);
            m_index_Buffer.value().bind();
            m_vertex_buffers.bind(0);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_instance_count.value());
            CHECK_GL_ERROR();
        }else if (m_index_Buffer.has_value() && !m_instance_count.has_value()){
            m_shader_manager.use_shader(m_shader_id, shader_uniforms);
            m_index_Buffer.value().bind();
            m_vertex_buffers.bind(0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            CHECK_GL_ERROR();
        }else{
            CORE_ERROR("Render method unsuported");
            exit(1);
        }
    }
            struct DataSet{
                bool index_buffer;
                std::optional<uint32_t> sub_buffer_index;
                uint32_t index;
                uint32_t size;
                void *data;
            };
    void OpenGLMaterial::set_data(DataSet& data){
        if (data.index_buffer || !data.sub_buffer_index.has_value()){
            //TODO
            CORE_ERROR("index buffer data change not supported yet");
            exit(1);
        }else if (data.sub_buffer_index.has_value()){
            m_vertex_buffers.bind(data.sub_buffer_index.value());
            glBufferSubData(GL_ARRAY_BUFFER, data.index, data.size, data.data);
            CHECK_GL_ERROR();
        }
    }
}