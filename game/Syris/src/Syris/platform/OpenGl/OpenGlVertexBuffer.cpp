#include "OpenGLVertexBuffer.hpp"
#include "Syris/log/Log.h"
#include <iostream>

namespace Syris::renderer { 
    OpenGLVertexBuffer::OpenGLVertexBuffer(CreateInfo info){    
        glGenVertexArrays(1, &m_vertex_array);
        if (m_vertex_array == NotSet){
            CORE_ERROR("Failed to generate vertex array");
            exit(1);
        }
        glBindVertexArray(m_vertex_array);
        m_buffers.resize(info.buffers_info.size());
        int buffer_index = 0;
        for (auto& buffer_info : info.buffers_info){
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
                glVertexAttribPointer(layout.index_layout, layout.values_count, GL_FLOAT, layout.normalize, layout.stride_size, (void*)layout.skip_size);
                if (layout.perInstance)
                    glVertexAttribDivisor(layout.index_layout,1);
                glEnableVertexAttribArray(layout.index_layout);

//                glVertexAttribDivisor(layout.index_layout, static_cast<GLuint>(layout.perInstance));
            }
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                CORE_ERROR(std::format("OpenGL error: {}", error));
                exit(1);
            }
            buffer_index++;
        }
    }
    
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        for (auto& buffer : m_buffers)
            glDeleteBuffers(1, &buffer);
    }

    void OpenGLVertexBuffer::bind(uint32_t index){
        glBindVertexArray(m_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER,m_buffers[index]);
    }

    void OpenGLVertexBuffer::unbind(uint32_t index){
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

}
