#pragma once
#include "Syris/app/Window.hpp"
#include "Syris/app/OpenGLWindow.hpp"
#include "Syris/context/GraphicsContext.hpp"
#include "Syris/layers/LayerManager.hpp"
#include "Syris/shader/ShaderManger.hpp"

namespace Syris{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GraphicsContext::CreateInfo& info);
        ~OpenGLContext() {};

        OpenGLContext()=delete;
        OpenGLContext(const OpenGLContext &ref) = delete;
        OpenGLContext &operator=(const OpenGLContext &ref) = delete;

        bool on_event(Event* event)override;
        void on_update(const engine_time::Time& time)override;
        bool should_window_close() override;
        //GLFWwindow* get_window() override {return m_window.get_window();}
        Window* get_window_handler() override {return m_window.get();}
        LayerManager& get_layer_manager()override{return m_layer_manager;}
        ShaderManager& get_shader_manager() override {return m_shader_manager;}
        int get_program(){return m_program;}
    private:
        std::unique_ptr<OpenGLWindow> m_window;
        int m_program;
        ShaderManager m_shader_manager;
        LayerManager m_layer_manager;

        //fps
        engine_time::FPSOverlay m_fps;
    };
}