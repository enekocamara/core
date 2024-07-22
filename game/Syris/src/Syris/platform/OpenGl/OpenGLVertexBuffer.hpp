#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Syris/renderer/VertexBuffer.hpp"

namespace Syris{ 

    constexpr auto NotSet = std::numeric_limits<GLuint>::max(); 
    class OpenGLVertexBuffer : public VertexBuffer{
        public:
            OpenGLVertexBuffer(CreateInfo info);
            ~OpenGLVertexBuffer();

            void bind()override;
            void unbind()override;
        private:
            GLuint m_vertex_array = std::numeric_limits<GLuint>::max();
            GLuint m_vertex_buffer = std::numeric_limits<GLuint>::max();
    };
}
