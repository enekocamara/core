#include "Syris/app/OpenGLWindow.h"
#include "Syris/layers/LayerManager.hpp"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/context/OpenGLContext.hpp"
/*OpenGL implementation of graphics context
*/
namespace Syris
{
    GraphicsContext::GraphicsContext(CreateInfo& info){
       // #ifdef RENDER_API_OPENGL
        m_context_impl = new OpenGLContext(info); 
       // #endif
    }
    GraphicsContext::~GraphicsContext(){
        delete m_context_impl;
    }
/*  bool GraphicsContext::should_window_close(){
        m_context_impl->should_window_close();
    }*/
}
