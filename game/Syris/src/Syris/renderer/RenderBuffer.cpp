#include "RenderBuffer.hpp"
#include <array>
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/context/OpenGLContext.hpp"
namespace Syris::renderer{
    RenderBuffer::RenderBuffer(CreateInfo info){
        std::array<VertexBuffer::BufferInfo, 2> buffers{info.per_vertex_buffer_info, info.per_instance_buffer_info};
        VertexBuffer::CreateInfo vertex_buffer_info = VertexBuffer::CreateInfo{
            .dynamic = info.dynamic,
            .buffers_info = {buffers.begin(), buffers.end()},
        };
        m_vertexBuffer = VertexBuffer::create(vertex_buffer_info);

        IndexBuffer::CreateInfo index_buffer_info{
            .indices_count = info.index_count,
            .indices = info.indices,
            .dynamic = info.dynamic,
        };
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
   /* void RenderBuffer::draw_buffer(Shader* shader, glm::mat4 projection_view, texture::Texture2D texture, uint32_t instances){

        //OpenGLContext* context_opengl = reinterpret_cast<OpenGLContext*>(context);
        shader->use();
        m_vertexBuffer->bind(1);
        //m_vertexBuffer->bind(1);
        m_indexBuffer->bind();
        shader->set_uniform_value(projection_view, "ViewProjection");
        //renderAPI::set_uniform_value(context_opengl->get_program(), projection_view, "ViewProjection");
        //glActiveTexture(GL_TEXTURE0);!!TODO
        texture.bind();

        shader->set_uniform1i(0,"texture1");
//        glUniform1i(glGetUniformLocation(context_opengl->get_program(), "texture1"), 0);
        shader->set_uniform(glm::vec3(1.f), "spriteColor");
        //renderAPI::set_uniform_value(context_opengl->get_program(), glm::vec3(1.f), "spriteColor");
        CHECK_GL_ERROR();
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instances);
        CHECK_GL_ERROR();
    }
    */
}
