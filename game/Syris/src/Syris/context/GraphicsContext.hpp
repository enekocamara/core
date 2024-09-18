#pragma once
#include <glm/glm.hpp>

#include "Syris/app/OpenGLWindow.h"
#include "Syris/layers/LayerManager.hpp"
#include "Syris/app/Window.hpp"
#include "Syris/shader/ShaderManger.hpp"
namespace Syris
{
    class IGraphicsContext{
        public:
        IGraphicsContext(){};
        virtual ~IGraphicsContext(){};
        virtual bool on_event(Event* event) = 0;
        virtual void on_update(engine_time::Time& time) = 0;
        virtual bool should_window_close() = 0;
        virtual GLFWwindow* get_window() = 0;
        virtual OpenGLWindow& get_window_handler() = 0;
        virtual LayerManager& get_layer_manager() = 0;
        virtual ShaderManager& get_shader_manager() = 0;

    };

    class GraphicsContext : public IGraphicsContext{
        public:
            struct CreateInfo
            {
                LayerManager::CreateInfo layers_info;
                WindowCreateInfo window_info;
               // const char* vertex_shader_path;
               // const char* fragment_shader_path;
                // resizeable
            };
            GraphicsContext(CreateInfo& info);
            ~GraphicsContext();
            bool on_event(Event* event) override {return m_context_impl->on_event(event);}
            void on_update(engine_time::Time& time) override {m_context_impl->on_update(time);}
            bool should_window_close() override {return m_context_impl->should_window_close();}
            LayerManager& get_layer_manager()override {return m_context_impl->get_layer_manager();}
            GLFWwindow* get_window() override {return m_context_impl->get_window();}
            OpenGLWindow& get_window_handler() override {return m_context_impl->get_window_handler();}
            ShaderManager& get_shader_manager() override {return m_context_impl->get_shader_manager();}
        private:
            IGraphicsContext *m_context_impl;
    };
}
/*{
public:
    struct CreateInfo{
        glm::ivec2 dimmensions;
        std::string name;
        bool vSync;
        //resizeable
    };
    GraphicsContext();
    GraphicsContext(const GraphicsContext &ref) = delete;
    GraphicsContext &operator=(const GraphicsContext &ref) = delete;
    ~GraphicsContext();
private:
    LayerManager m_layerManager;
    //depending on the template parameter it will be casted to OpenGLContext* or VulcanContext
    void *m_context_impl;
};
*/