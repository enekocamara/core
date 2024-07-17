#pragma once
#include <iostream>
#include <glm/glm.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Window{
    public:
        struct WindowBundle{
            glm::ivec2 dimmensions;
            std::string name;
            bool vSync;
        };
        Window(WindowBundle info);
        ~Window();
        void init();
        void shutDown();
        void onUpdate();
    private:
        GLFWwindow *m_window;
        WindowBundle m_info;
};
