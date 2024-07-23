#include "RenderBuffer.hpp"
#include <array>
#include "Syris/renderAPI/OpenGl/renderApi.h"
namespace Syris::renderer{
    RenderBuffer::RenderBuffer(CreateInfo info){
        std::array<VertexBuffer::BufferInfo, 2> buffers{info.per_vertex_buffer_info, info.per_instance_buffer_info};
        VertexBuffer::CreateInfo vertex_buffer_info = {};
        vertex_buffer_info.dynamic = info.dynamic;
        vertex_buffer_info.buffers_info = {buffers.begin(), buffers.end()};
        m_vertexBuffer = VertexBuffer::create(vertex_buffer_info);

        IndexBuffer::CreateInfo index_buffer_info = {};
        index_buffer_info.dynamic = info.dynamic;
        index_buffer_info.indices = info.indices;
        index_buffer_info.indices_count = info.index_count;
        m_indexBuffer = IndexBuffer::create(index_buffer_info);
    }
    RenderBuffer::~RenderBuffer(){
        delete m_vertexBuffer;
        delete m_indexBuffer;
    }
    void RenderBuffer::bind(uint32_t buffer_index){
        m_vertexBuffer->bind(buffer_index);
        m_indexBuffer->bind();
    }
    void RenderBuffer::set_data(void *data){
         
    }
    void RenderBuffer::update_data(void *data){
         
    }
    void RenderBuffer::draw_buffer(GLuint program, glm::mat4 projection_view, texture::Texture2D texture, uint32_t instances){
        m_vertexBuffer->bind(1);
        //m_vertexBuffer->bind(1);
        m_indexBuffer->bind();
        renderAPI::set_uniform_value(program, projection_view, "ViewProjection");
        glActiveTexture(GL_TEXTURE0);
        texture.bind();
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);
        renderAPI::set_uniform_value(program, glm::vec3(1.f), "spriteColor");
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << err << std::endl;
        }
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instances);
    }
}
