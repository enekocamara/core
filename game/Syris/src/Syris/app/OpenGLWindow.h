#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include <imgui.h>
#include "../platform/OpenGl/imgui_impl_glfw.h"
#include "../platform/OpenGl/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "Syris/layers/Layer.hpp"

namespace Syris{
    class OpenGLWindow{
        public:
            OpenGLWindow(WindowCreateInfo& info);
            OpenGLWindow()=delete;
            OpenGLWindow& operator=(const OpenGLWindow& ref) = delete;
            ~OpenGLWindow();
            void init();
            void shut_down();
            void on_update_start();
            void on_update_end();
            int should_window_close(){return glfwWindowShouldClose(m_window);}
            GLFWwindow* get_window(){return m_window;}
            void swap_buffers();
        private:
            class OpenGLLayer{};
            OpenGLLayer* m_window_layer;
            GLFWwindow* m_window;
            WindowCreateInfo m_info;
    };

    class OpenGLLayer : public Layer
    {
    public:
        OpenGLLayer(OpenGLWindow& window) : m_window(window){};
        ~OpenGLLayer() {};
        void on_update() override;
        bool on_event(Event* event)override;

    private:
        OpenGLWindow &m_window;
    };
}
