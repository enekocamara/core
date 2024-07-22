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

namespace Syris{
    class Window{
        public:
            struct WindowBundle{
                glm::ivec2 dimmensions;
                std::string name;
                bool vSync;
            };
            Window()=delete;
            Window& operator=(const Window& ref) = delete;
            Window(WindowBundle info);
            ~Window();
            void init();
            void shutDown();
            void onUpdateStart();
            void onUpdateEnd();
            int shouldWindowClose(){return glfwWindowShouldClose(m_window);}
            GLFWwindow* getWindow(){return m_window;}
        private:
            GLFWwindow *m_window;
            WindowBundle m_info;
    };
}
