#pragma once
#include <string>
#include "Syris/context/GraphicsContext.hpp"
#include "EnginePortal.hpp"
#include "Syris/app/Application.hpp"
#include "EngineState.hpp"
#include "NewProjectPromptUI.hpp"

namespace GameEngine{
    class EngineApp : public Syris::Application{
        public:
            EngineApp(Syris::Statistics& statistics);
            ~EngineApp(){}
            EngineApp(const EngineApp& ref) = delete;
            EngineApp& operator=(const EngineApp& ref) = delete;
            void run()override;
        private:
            void update_ui();

        
            std::unique_ptr<Syris::GraphicsContext> m_graphics_context;
            EngineState m_state;
            EnginePortal m_portal;
            NewProjectPromptUI m_new_project_promp_ui;
            Syris::LayerID m_current_layer;
    };
}