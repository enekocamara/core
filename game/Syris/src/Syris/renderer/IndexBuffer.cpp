#include "IndexBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLIndexBuffer.hpp"
namespace Syris{

    void IndexBuffer::bind(){
        
    }
    void IndexBuffer::unbind(){

    }

    IndexBuffer* IndexBuffer::create(CreateInfo info){
        return  new OpenGLIndexBuffer(info); 
    }
}
