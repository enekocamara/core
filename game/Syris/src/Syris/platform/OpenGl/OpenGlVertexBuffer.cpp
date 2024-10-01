#include "OpenGLVertexBuffer.hpp"
#include "Syris/log/Log.h"
#include <iostream>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"

namespace Syris { 
    OpenGLVertexBuffer::OpenGLVertexBuffer(CreateInfo info){    
        
        glGenVertexArrays(1, &m_vertex_array);
        if (m_vertex_array == NotSet){
            CORE_ERROR("Failed to generate vertex array");
            exit(1);
        }
        glBindVertexArray(m_vertex_array);
        m_buffers.reserve(info.buffers_info.size());
        int buffer_index = 0;
        for (auto buffer_info : info.buffers_info){
            m_buffers.emplace_back(buffer_info);/*
            m_buffers[buffer_index] = NotSet;
            glGenBuffers(1, &m_buffers[buffer_index]);
            if (m_buffers[buffer_index] == NotSet){
                std::cerr << "error setting buffers\n";
                exit(1);
            }
            glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer_index]);
            if (info.dynamic)
                glBufferData(GL_ARRAY_BUFFER, buffer_info.size, buffer_info.data, GL_DYNAMIC_DRAW);
            else
                glBufferData(GL_ARRAY_BUFFER, buffer_info.size, buffer_info.data, GL_STATIC_DRAW);

            for (uint32_t i = 0; i < buffer_info.layout_list.size(); i++){
                auto& layout = buffer_info.layout_list[i];
                if (layout.value_type != ValueType::Float){
                    CORE_ERROR("TODO! Value type different to float");
                    exit(1);
                }
                switch (layout.value_type){
                    case ValueType::Float:
                        glVertexAttribPointer(layout.index_layout, layout.values_count, GL_FLOAT, layout.normalize, layout.stride_size, (void*)layout.skip_size);
                        break; 
                    case ValueType::Int:
                        glVertexAttribPointer(layout.index_layout, layout.values_count, GL_INT, layout.normalize, layout.stride_size, (void*)layout.skip_size);
                        break; 
                    default:
                        CORE_ERROR("TODO! Value type different to float or int");
                        exit(1);
                } 
                glEnableVertexAttribArray(layout.index_layout);
                if (layout.perInstance)
                    glVertexAttribDivisor(layout.index_layout, 1);

//                glVertexAttribDivisor(layout.index_layout, static_cast<GLuint>(layout.perInstance));
                //std::cout << "index  layout "  << layout.index_layout << "\n\t"<<'\n';
            }
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                CORE_ERROR(std::format("OpenGL error: {}", error));
                exit(1);
            }
            buffer_index++;
            */
        }
        
    }

    void OpenGLVertexBuffer::append(uint32_t index, uint64_t size, void* data){
        m_buffers[index].append(size, data);
    }
    
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        //for (auto& buffer : m_buffers)
        //    glDeleteBuffers(1, &buffer);
    }

    void OpenGLVertexBuffer::bind(uint32_t index){
        glBindVertexArray(m_vertex_array);
        m_buffers[index].bind();
    }

    void OpenGLVertexBuffer::unbind(uint32_t index){
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }


    OpenGLSubBuffer::OpenGLSubBuffer(VertexBuffer::SubBufferInfo info):m_info(info){
        m_buffer_id = gen_buffer(info);
    }

    OpenGLSubBuffer::~OpenGLSubBuffer(){
        //todo free memory
    }

    GLuint OpenGLSubBuffer::gen_buffer(VertexBuffer::SubBufferInfo& info){
        GLuint buffer_id;
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        if (info.dynamic)
            glBufferData(GL_ARRAY_BUFFER, info.size, info.data, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, info.size, info.data, GL_STATIC_DRAW);

        for (uint32_t i = 0; i < info.layout_list.size(); i++)
        {
            auto &layout = info.layout_list[i];
            if (layout.value_type != ValueType::Float)
            {
                CORE_ERROR("TODO! Value type different to float");
                exit(1);
            }
            switch (layout.value_type)
            {
            case ValueType::Float:
                glVertexAttribPointer(layout.index_layout, layout.values_count, GL_FLOAT, layout.normalize, layout.stride_size, (void *)layout.skip_size);
                break;
            case ValueType::Int:
                glVertexAttribPointer(layout.index_layout, layout.values_count, GL_INT, layout.normalize, layout.stride_size, (void *)layout.skip_size);
                break;
            default:
                CORE_ERROR("TODO! Value type different to float or int");
                exit(1);
            }
            glEnableVertexAttribArray(layout.index_layout);
            if (layout.perInstance)
                glVertexAttribDivisor(layout.index_layout, 1);
        }
        CHECK_GL_ERROR();
        return buffer_id;
    }

    void OpenGLSubBuffer::bind(){
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
        CHECK_GL_ERROR();
    }

    /*size is the new total  size in bytes*/
    void OpenGLSubBuffer::reserve(uint64_t size){
        while (size > m_capacity){
            m_capacity *= 2;
        }
        GLuint new_buffer = gen_buffer(m_info);
        glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);  // Bind old buffer as source
        glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer); // Bind new buffer as destination
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_info.size);
        glDeleteBuffers(1, &m_buffer_id);
        CHECK_GL_ERROR();

        m_buffer_id = new_buffer;
    }
    void OpenGLSubBuffer::append(uint64_t size, void *data){
        if (m_info.size + size < m_capacity){
            reserve(size + m_info.size);
        }
        glBufferSubData(GL_ARRAY_BUFFER, m_info.size, size, data);
        m_info.size += size;
        CHECK_GL_ERROR();
    }
    void OpenGLSubBuffer::clear_part(uint64_t index, uint64_t size){
        //todo
    }
}
