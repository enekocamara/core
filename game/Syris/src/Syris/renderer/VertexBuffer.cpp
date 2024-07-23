#include "VertexBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"

namespace Syris::renderer{
    VertexBuffer* VertexBuffer::create(CreateInfo info){ 
        return new OpenGLVertexBuffer(info);
    }
}
