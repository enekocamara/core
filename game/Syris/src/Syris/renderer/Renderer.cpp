#include "Renderer.hpp"

namespace Syris{
    void Renderer::push_framebuffer(FrameBuffer* framebuffer){
        get_renderer().m_framebuffers.push(framebuffer);
        framebuffer->bind();
    }
    FrameBuffer* Renderer::pop_framebuffer(){
        auto& framebuffer = get_renderer().m_framebuffers.top();
        get_renderer().m_framebuffers.pop();
        get_renderer().m_framebuffers.top()->bind();
        return framebuffer;
    }
    FrameBuffer* Renderer::get_current_framebuffer(){
        return get_renderer().m_framebuffers.top();
    }
    void Renderer::push_viewport(glm::uvec2 viewport){
        get_renderer().m_viewports.push({viewport});
        glViewport(0,0, viewport.x, viewport.y);
    }
    ViewPort Renderer::pop_viewport(){
        auto& viewport = get_renderer().m_viewports.top();
        get_renderer().m_viewports.pop();
        get_renderer().m_viewports.top().set(); 
        return viewport;

    }
    ViewPort Renderer::get_current_viewport(){
        return get_renderer().m_viewports.top();
    }
    Renderer &Renderer::get_renderer(){
        static Renderer renderer;
        return renderer;
    }
}