#pragma once
#include <glm/glm.hpp>
#include "Syris/statistics/Statistics.hpp"
namespace Syris{
    class Window
    {
    public:
        struct CreateInfo
        {
            glm::ivec2 dimmensions;
            std::string name;
            bool vSync;
            Statistics& statistics;
        };
        virtual ~Window(){};
        virtual  void on_update_start() = 0;
        virtual void on_update_end() = 0;
        virtual bool should_window_close() = 0;
        virtual ImGuiContext *get_imgui_context() = 0;
        virtual void swap_buffers() = 0;
        static std::unique_ptr<Window> new_window(CreateInfo& info);
    };
}