#include "OpenGLWindow.hpp"
#include "Syris/log/Log.h"
#include "imgui.h"

namespace Syris{
    void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
            std::cerr << "OpenGL Debug Message: " << message << std::endl;
        if (severity == GL_DEBUG_SEVERITY_HIGH)
        {
            CORE_ERROR(message);
            throw std::runtime_error(message);
            exit(1);
        }
    }
    OpenGLWindow::OpenGLWindow(CreateInfo& info): m_info(info){
        init();
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    OpenGLWindow::~OpenGLWindow(){
        shut_down();
    }
    void OpenGLWindow::init(){
        int code = glfwInit();
        if (code != GLFW_TRUE){
            CORE_ERROR(std::format("Failed to init window: error code: {}", code));
            exit(1);
        }

        const char* glsl_version = "#version 460";//exists in imguilayer
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(m_info.dimmensions.x, m_info.dimmensions.y, m_info.name.c_str(), nullptr, nullptr);
        if (!m_window){
            CORE_ERROR("Failed to create window");
            exit(1);
        }
        glfwMakeContextCurrent(m_window);
        int status  = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!status){
            CORE_ERROR("Failed to load gl loader");
            exit(1);
        }
        glfwSetWindowUserPointer(m_window, &m_info);
        glfwSwapInterval((int)m_info.vSync);

        IMGUI_CHECKVERSION();
        m_gui_context = ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    }
    void OpenGLWindow::shut_down(){
        glfwDestroyWindow(m_window);
    }
    void OpenGLWindow::on_update_start(){
        bool show_demo_window = false;
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    void OpenGLWindow::on_update_end(){

    }
    void OpenGLWindow::swap_buffers(){
        glfwSwapBuffers(m_window);
    }
}
