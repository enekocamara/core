#include "EngineState.hpp"

namespace GameEngine{
    EngineState::EngineState(Syris::Statistics &statistics, Syris::GraphicsContext &graphics_context) : m_graphics_context(graphics_context)
    {
    }
    void EngineState::show_new_project_prompt_ui()
    {
        m_current_state = State::NewProjectPromptUI;
    }
    void EngineState::begin_new_project(std::string project_name)
    {
        m_current_state = State::InProject;
    }
}