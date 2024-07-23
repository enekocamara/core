#pragma once

#include "AttributeLayoutList.hpp"
#include "Syris/renderer/Texture.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

namespace Syris::renderer{
    class RenderBuffer{
        public:
            /*
             *@brief hola
             *hola?
             *
             * */
            struct CreateInfo{
                bool dynamic;
                uint32_t index_count;
                uint32_t *indices;
                VertexBuffer::BufferInfo per_vertex_buffer_info;
                VertexBuffer::BufferInfo per_instance_buffer_info;
            };
            RenderBuffer(CreateInfo info);
            ~RenderBuffer();
            void bind(uint32_t index);
            void set_data(void *data);
            void update_data(void *data);
            void draw_buffer(GLuint program, glm::mat4 projection_matrix, texture::Texture2D texture, uint32_t instances);
        private:
            /*uint32_t size;
            uint32_t capacity;
            */
            VertexBuffer* m_vertexBuffer;
            IndexBuffer* m_indexBuffer;
            AttributeLayoutList layouts = {0};
    };
}
