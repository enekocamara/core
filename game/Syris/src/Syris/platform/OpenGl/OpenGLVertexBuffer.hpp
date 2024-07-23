#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Syris/renderer/VertexBuffer.hpp"

namespace Syris::renderer{ 

    constexpr auto NotSet = std::numeric_limits<GLuint>::max(); 
    class OpenGLVertexBuffer : public VertexBuffer{
        public:
            OpenGLVertexBuffer(CreateInfo info);
            ~OpenGLVertexBuffer();

            void bind(uint32_t index)override;
            void unbind(uint32_t index)override;
        private:
            GLuint m_vertex_array = std::numeric_limits<GLuint>::max();
            std::vector<GLuint> m_buffers;
    };
}
