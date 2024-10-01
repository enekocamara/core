#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Syris/renderer/VertexBuffer.hpp"

namespace Syris{ 

    class OpenGLSubBuffer{
        public:
            OpenGLSubBuffer(VertexBuffer::SubBufferInfo info);
            ~OpenGLSubBuffer();
            uint64_t size();
            uint64_t instance_count();
            void bind();
            void reserve(uint64_t size);
            void append(uint64_t size, void *data);
            void clear_part(uint64_t index, uint64_t size);
            GLuint gen_buffer(VertexBuffer::SubBufferInfo& info);
        private:
            uint64_t m_capacity;
            GLuint m_buffer_id;
            VertexBuffer::SubBufferInfo m_info;
    };

    constexpr auto NotSet = std::numeric_limits<GLuint>::max(); 
    class OpenGLVertexBuffer : public VertexBuffer{
        public:
            OpenGLVertexBuffer(CreateInfo info);
            ~OpenGLVertexBuffer();

            void bind(uint32_t index)override;
            void unbind(uint32_t index)override;
            void append(uint32_t index, uint64_t size, void* data)override;
        private:
            GLuint m_vertex_array = std::numeric_limits<GLuint>::max();
            //std::vector<GLuint> m_buffers;
            std::vector<OpenGLSubBuffer> m_buffers;
    };
}
