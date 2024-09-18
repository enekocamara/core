#pragma once
#include "Syris/include/imgui.h"

namespace Syris{
    //commun interface for imgui context to sincronize projects
    //ensure that two different context are not used
    class ImGuiContext{
        public:
            ImGuiContext();
            ~ImGuiContext();
            ImGuiContext* get_context(){return m_context;}
        private:
            ImGuiContext *m_context;
    };
}