#pragma once
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/utils/Debug.h"

/*
    ui_manager->state_machine
    ui->state_machine
*/

namespace GameEngine{
    class EngineState
    {
    public:
        enum class State{
            Portal,
            NewProjectPromptUI,
            InProject
        };
        EngineState(Syris::Statistics &statistics, Syris::GraphicsContext& graphics_context);
        EngineState() = delete;
        EngineState(const EngineState &ref) = delete;
        EngineState &operator=(const EngineState &ref) = delete;

        Syris::GraphicsContext& get_graphics_context(){return m_graphics_context;}
        void show_new_project_prompt_ui();
        void begin_new_project(std::string project_name);
        void change_state(State new_state){TODO();}
        State get_state()const{return m_current_state;}
    private:
        Syris::GraphicsContext& m_graphics_context;
        State m_current_state = State::Portal;
    };
}