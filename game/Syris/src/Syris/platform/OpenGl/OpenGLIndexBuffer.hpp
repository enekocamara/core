#pragma once

#include "Syris/renderer/IndexBuffer.hpp"
#include <glad/glad.h>

namespace Syris{
    class OpenGLIndexBuffer: public IndexBuffer{
        public:
            OpenGLIndexBuffer(IndexBuffer::CreateInfo info);
            ~OpenGLIndexBuffer();
            void bind();
            void unbind();
        private:
            GLuint m_buffer;
    };
}
