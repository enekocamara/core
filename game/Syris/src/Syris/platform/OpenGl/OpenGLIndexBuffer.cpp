#include "OpenGLIndexBuffer.hpp"

namespace Syris{

    OpenGLIndexBuffer::OpenGLIndexBuffer(IndexBuffer::CreateInfo info){
        glCreateBuffers(1, &m_buffer);      
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        if (info.dynamic)
            glBufferData(GL_ARRAY_BUFFER, info.indices_count * sizeof(uint32_t), info.indices, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, info.indices_count * sizeof(uint32_t), info.indices, GL_STATIC_DRAW);
        
    }
    OpenGLIndexBuffer::~OpenGLIndexBuffer(){

    }
    void OpenGLIndexBuffer::bind(){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_buffer);
    }
    void OpenGLIndexBuffer::unbind(){

    }
}
