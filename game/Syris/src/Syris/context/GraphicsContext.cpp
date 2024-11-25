#include "Syris/app/Window.hpp"
#include "Syris/layers/LayerManager.hpp"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/context/OpenGLContext.hpp"
/*OpenGL implementation of graphics context
*/
namespace Syris
{
    std::unique_ptr<GraphicsContext> GraphicsContext::new_context(CreateInfo& info){
        return std::make_unique<OpenGLContext>(info); 
    }
}
