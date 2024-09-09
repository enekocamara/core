#include "OpenGLcontext.hpp"
#include "Syris/shader/OpenGLShader.hpp"

namespace Syris{
    OpenGLContext::OpenGLContext(GraphicsContext::CreateInfo &info) : m_layer_manager(info.layers_info), m_window(info.window_info)
    {
        m_program = glCreateProgram();
        //compile_shaders(m_program, info.vertex_shader_path,
         //                      info.fragment_shader_path);
    }
    bool OpenGLContext::should_window_close(){
        return m_window.should_window_close();
    }
    void OpenGLContext::on_update(){
        glfwPollEvents();
        //m_shader_manager.use_shader((uint32_t)0);
        m_layer_manager.on_update();
        m_window.swap_buffers();
        //std::cout << "swap buffers\n";
    }
    bool OpenGLContext::on_event(Event* event){
        return m_layer_manager.on_event(event);
    }
}