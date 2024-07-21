#include "App.h"
#include "../renderer/Shader.hpp"
namespace app{
    App::App(AppBundle& bundle, scene::Scene *scene) : m_window(bundle.window_bundle),
                                                       m_scene(scene),
                                                       m_renderer(bundle.registry),
                                                       m_registry(bundle.registry){
        m_glId = glCreateProgram(); 
        shader::compile_shader(m_glId, bundle.vertex_shader_path, bundle.fragment_shader_path); 
    }
    void App::run(){
/*        while(!m_window.shouldWindowClose()){
            m_window.onUpdate();
//            m_scene->draw_frame(m_renderer);
        }*/
    }
}
