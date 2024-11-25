#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Syris/context/GraphicsContext.hpp"
#include "Syris/texture/Texture.hpp"
#include "Syris/shader/Shader.hpp"

#include "AttributeLayoutList.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace Syris{
    //this will be opengl render buffer later
    /*
    class RenderBuffer{
        public:
            struct CreateInfo{
                bool dynamic;
                uint32_t index_count;
                uint32_t *indices;
                VertexBuffer::SubBufferInfo per_vertex_buffer_info;
                VertexBuffer::SubBufferInfo per_instance_buffer_info;
                Statistics& statistics;
            };
            RenderBuffer(CreateInfo info);
            ~RenderBuffer();
            void bind(uint32_t index);
            void set_data(void *data);
            void update_data(void *data);
            //void draw_buffer(Shader* shader, glm::mat4 projection_matrix, texture::Texture2D texture, uint32_t instances);
        private:
            VertexBuffer* m_vertexBuffer;
            IndexBuffer* m_indexBuffer;
            AttributeLayoutList layouts = {0};
    };
    */
}
