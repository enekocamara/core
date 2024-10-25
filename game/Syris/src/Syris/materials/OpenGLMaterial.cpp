#include "OpenGLMaterial.hpp"
#include "Syris/log/Log.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "MatRequestQueue.hpp"

namespace Syris {

    /*************** OPENGL MATERIAL IMPLEMENTATION ******************/
    /*
    template<typename T>
    OpenGLMaterial<T>::OpenGLMaterial(CreateInfo& info):
        m_shader_manager(info.shader_manager),
        m_shader_id(info.shader_id),
        m_vertex_buffers(info.vertex_buffer_info),
        m_instance_count(info.instance_count)
    {

        if (info.index_buffer_info.has_value()){
            m_vertex_buffers.bind();
            m_index_Buffer.emplace(info.index_buffer_info.value());
        }
        if (!m_index_Buffer.has_value() && !m_instance_count.has_value()){
            CORE_ERROR("Render method unsuported");
            exit(1);
        }
    }

    template<typename T>
    OpenGLMaterial<T>::~OpenGLMaterial(){
        //todo
    }


    template<typename T>
    void OpenGLMaterial<T>::draw(void *shader_uniforms){
        //m_vertex_buffers.print_info();
        flush_add_entity();
        flush_set_entity();
        CHECK_GL_ERROR();
        if (m_index_Buffer.has_value() && m_instance_count.has_value()){

            m_shader_manager.use_shader(m_shader_id, shader_uniforms);
            m_vertex_buffers.bind();
            m_index_Buffer.value().bind();

            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_instance_count.value());
            CHECK_GL_ERROR();

        }else if (m_index_Buffer.has_value() && !m_instance_count.has_value()){
            m_shader_manager.use_shader(m_shader_id, shader_uniforms);
            m_index_Buffer.value().bind();
            m_vertex_buffers.bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            CHECK_GL_ERROR();
        }else{
            CORE_ERROR("Render method unsuported");
            exit(1);
        }
    }
    template<typename T>
    void OpenGLMaterial<T>::set_entity(MaterialSetRequest& request){
        m_requests_queues.set(request);
    }
    template<typename T>
    void OpenGLMaterial<T>::add_entity(MaterialAddRequest& request){
        m_requests_queues.add_entity(request);
    }

    template<typename T>
    void OpenGLMaterial<T>::flush_add_entity(){
        Queue<T> add_request = m_requests_queues.get_add_queue();
        //subbuffer will need to be specified later
        uint64_t start = m_entity_to_index.size();
        for (auto [entity, index] : add_request.m_entity_to_index){
            m_entity_to_index[entity] = start + index;
        }
        m_vertex_buffers.append(1, add_request.m_request_data.size, add_request.m_request_data.data());
        //m_vertex_buffers.append(subbufer_index, size, data);
        if (m_instance_count.has_value()){
            m_instance_count.value() += add_request.m_entity_to_index.size();
        }
        m_requests_queues.clear_append_queue();
    }

    template<typename T>
    void OpenGLMaterial<T>::flush_set_entity(){
        auto set_requests = m_requests_queues.get_set_queue();
        for (auto set_request : set_requests){
            if (set_request.index_buffer || !set_request.sub_buffer_index.has_value())
            {
                // TODO
                CORE_ERROR("index buffer data change not supported yet");
                exit(1);
            }
            else if (set_request.sub_buffer_index.has_value())
            {
                m_vertex_buffers.bind_subbuffer(set_request.sub_buffer_index.value());
                glBufferSubData(GL_ARRAY_BUFFER, set_request.mem_offset, set_request.size, set_request.data);
                CHECK_GL_ERROR();
            }
        }
        m_requests_queues.clear_set_queue();
    }

    template<typename T>
    void OpenGLMaterial<T>::remove_entity(MaterialRemoveRequest& request){
        m_requests_queues.pop(request);
    }

    template<typename T>
    void OpenGLMaterial<T>::flush_remove_entity(){
    }

*/
}