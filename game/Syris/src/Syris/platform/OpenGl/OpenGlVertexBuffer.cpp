#include "OpenGLVertexBuffer.hpp"
#include "Syris/log/Log.h"
namespace Syris { 
    OpenGLVertexBuffer::OpenGLVertexBuffer(CreateInfo info){    
        Logger::core_trace("creating vertex buffer");
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

        if (info.dynamic)
            glBufferData(GL_ARRAY_BUFFER, info.vertice_size * info.vertices_count, info.data, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, info.vertice_size * info.vertices_count, info.data, GL_STATIC_DRAW);

        for (uint32_t i = 0; i < info.layouts_count; i++){
            glEnableVertexAttribArray(info.layouts[i].index_layout);
            glVertexAttribPointer(info.layouts[i].index_layout, info.layouts[i].values_count, GL_FLOAT, info.layouts[i].normalize, info.layouts[i].stride_count * sizeof(float), &info.layouts[i].skip_count);
        }
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << error << std::endl;
            exit(1);
        }
        if (m_vertex_array == NotSet || m_vertex_buffer == NotSet){
            std::cerr << "error setting buffers\n";
            exit(1);
        }
        Logger::core_trace("vertex buffer succesfully created");
    }
    
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        glDeleteBuffers(1, &m_vertex_buffer);
    }

    void OpenGLVertexBuffer::bind(){
        if (m_vertex_array == NotSet || m_vertex_buffer == NotSet){
            std::cerr << "error setting buffers\n";
            exit(1);
        }
        glBindBuffer(GL_ARRAY_BUFFER,m_vertex_buffer);
        glBindVertexArray(m_vertex_array);
    }

    void OpenGLVertexBuffer::unbind(){
        if (m_vertex_array == NotSet || m_vertex_buffer == NotSet){
            std::cerr << "error setting buffers\n";
            exit(1);
        }
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

}
