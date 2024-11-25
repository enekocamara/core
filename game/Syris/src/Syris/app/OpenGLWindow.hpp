#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include "Syris/include/imgui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "Syris/layers/Layer.hpp"

namespace Syris{
    class OpenGLWindow : public Window
    {
    public:
        
        OpenGLWindow(CreateInfo& info);
        OpenGLWindow() = delete;
        OpenGLWindow &operator=(const OpenGLWindow &ref) = delete;
        ~OpenGLWindow();

        void init();
        void shut_down();

        void on_update_start();
        void on_update_end();

        bool should_window_close() { return glfwWindowShouldClose(m_window); }
        GLFWwindow *get_window() { return m_window; }

        ImGuiContext *get_imgui_context() { return m_gui_context; }
        void swap_buffers();

    private:
        // OpenGLLayer m_window_layer;
        GLFWwindow *m_window;
        CreateInfo m_info;
        ImGuiContext *m_gui_context;
    };
}
