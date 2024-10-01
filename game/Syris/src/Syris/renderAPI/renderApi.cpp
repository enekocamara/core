#include "renderApi.h"
#include "OpenGl/OpenGLrenderApi.h"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"
namespace Syris::renderAPI{
    void draw_quad(int program, Quad2D* quad, glm::mat4 model, glm::mat4 projection_view, texture::Texture2D texture, texture::Rectangle2D src, glm::vec3 color){ 
        opengl_draw_quad2D(program, static_cast<OpenGLVertexBuffer*>(quad->buffer), model, projection_view, texture, src, color);
    }
    Quad2D::Quad2D(VertexBuffer::CreateInfo info){
        buffer = VertexBuffer::create(info);
    }
    Quad2D::~Quad2D(){
        delete buffer;
    }
}
