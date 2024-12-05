#pragma once
#include <glm/glm.hpp>

#include "Syris/app/Window.hpp"
#include "Syris/layers/LayerManager.hpp"
#include "Syris/shader/ShaderManger.hpp"

namespace Syris
{
    class GraphicsContext : public Layer{
    public:
        struct CreateInfo
        {
            LayerManager::CreateInfo layers_info;
            Window::CreateInfo window_info;
            Statistics& statistics;
        };
        static std::unique_ptr<GraphicsContext> new_context(CreateInfo& info);
        virtual ~GraphicsContext() {};
        /*virtual bool on_event(Event* event) = 0;
        virtual void on_update(engine_time::Time &time) = 0;*/
        virtual bool should_window_close() = 0;
        //virtual GLFWwindow *get_window() = 0;
        virtual Window* get_window_handler() = 0;
        virtual LayerManager &get_layer_manager() = 0;
        virtual ShaderManager &get_shader_manager() = 0;
        virtual glm::uvec2 get_current_render_window_size() const = 0;
    };
}