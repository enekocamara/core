#include "EngineApp.hpp"

namespace GameEngine{
    EngineApp::EngineApp(Syris::Statistics& statistics):
        m_state(statistics, *m_graphics_context.get()),
        m_portal(m_state),
        m_new_project_promp_ui(m_state)
    {
        Syris::LayerManager::CreateInfo layer_info{
            .layers = {}
        };
        Syris::Window::CreateInfo window_info{
            .dimmensions = {1920,1080},
            .name = "Game AppEngine",
            .vSync = true,
            .statistics = statistics
        };
        Syris::GraphicsContext::CreateInfo info{
            .layers_info = layer_info,
            .window_info = window_info,
            .statistics = statistics
        };

        m_graphics_context = Syris::GraphicsContext::new_context(info);
        m_current_layer = m_graphics_context->get_layer_manager().push_layer(&m_portal);
    }
    void EngineApp::run(){
        Syris::engine_time::Time time;
        time.start();
        while(!m_graphics_context->should_window_close()){
            time.next_frame();
            update_ui();
            m_graphics_context->on_update(time);
        }
    }
    void EngineApp::update_ui(){
        switch(m_state.get_state()){
            case EngineState::State::Portal:
                m_current_layer =  m_graphics_context->get_layer_manager().replace_layer(&m_portal, m_current_layer);
                break;
            case EngineState::State::NewProjectPromptUI:
                m_current_layer =  m_graphics_context->get_layer_manager().replace_layer(&m_new_project_promp_ui, m_current_layer);
                break;
            case EngineState::State::InProject:
                break;
            default:
                BREAK_POINT("Unhandled engine state");
        }
    }
}
std::unique_ptr<Syris::Application> get_client_app(Syris::AppInit &app_init)
{
    return std::make_unique<GameEngine::EngineApp>(app_init.statistics);
}
