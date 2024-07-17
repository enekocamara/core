#include "App.h"
#include "../renderer/Shader.hpp"
namespace app{
    App::App(AppBundle& bundle) : m_window(bundle.window_bundle), m_scene(bundle.registry), m_renderer(m_scene, bundle.registry), m_registry(bundle.registry){
        m_glId = glCreateProgram(); 
        shader::compile_shader(m_glId, bundle.vertex_shader_path, bundle.fragment_shader_path); 
    }
    void App::run(){
        while(!m_window.shouldWindowClose()){
            m_window.onUpdate();
            m_renderer.renderFrame();
        }
    }
}
