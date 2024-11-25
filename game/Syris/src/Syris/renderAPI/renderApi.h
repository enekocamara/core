#pragma once
#include <glm/glm.hpp>

// #ifdef RENDER_API_OPENGL
#include "Syris/renderer/VertexBuffer.hpp"
#include "Syris/texture/Texture.hpp"

namespace Syris::renderAPI{

    struct Quad2D{
        Quad2D() = delete;
        Quad2D(const Quad2D& ref) = delete;
        Quad2D(VertexBuffer::CreateInfo);
        ~Quad2D();
        VertexBuffer* buffer = nullptr;
    }; 
    void draw_quad(int program, Quad2D* quad, glm::mat4 model, glm::mat4 projection_view, Texture2D& texture, Rectangle2D src, glm::vec3 color);
}
//#endif
