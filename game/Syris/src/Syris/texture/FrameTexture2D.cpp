#include "FrameTexture2D.hpp"
namespace Syris{
    FrameTexture2D::FrameTexture2D(CreateInfo info)
        : m_texture(info.texture),
          m_shader_manager(info.shader_manager),
          m_shader_id(info.shader_id),
          //m_size(info.size),
          m_frame_buffer(info.size){
    }

    void FrameTexture2D::render(){
        Renderer::push_framebuffer(&m_frame_buffer);
        Renderer::push_viewport({m_texture->get_size()});
        glClearColor(0.0f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Uniform uniform{
            .name = "uTexture",
            .data = m_texture,
            .pnext = nullptr
        };
        m_shader_manager.use_shader(m_shader_id, &uniform);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        Renderer::pop_framebuffer();
        Renderer::pop_viewport();
    }
}