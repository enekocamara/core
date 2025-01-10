#include "EnginePortal.hpp"
#include "Syris/include/imgui.h"
#include "Syris/utils/imgui/styles.h"

namespace GameEngine{
    EnginePortal::EnginePortal(EngineState& state):m_state(state)
    {
    }
    void EnginePortal::on_update(const Syris::engine_time::Time &time)
    {
        ImGui::Begin("Portal");
        if (Syris::centered_button("New Project"))
        {
            m_state.show_new_project_prompt_ui();
        }
        if (Syris::centered_button("Open Project"))
        {
        }
        ImGui::End();

    }
    bool EnginePortal::on_event(Syris::Event *event)
    {
        return false;
    }
}