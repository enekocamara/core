#include "OpenGLWindow.hpp"
#include "Syris/log/Log.h"
#include "Syris/utils/Breakpoint.h"
#include "imgui.h"

namespace Syris{
    void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
            CORE_ERROR(std::format("OpenGL Debug Message: {}", message));
        if (severity == GL_DEBUG_SEVERITY_HIGH){
            BREAK_POINT(message);
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
        if (code != GLFW_TRUE)
            BREAK_POINT(std::format("Failed to init window: error code: {}", code));

        const char* glsl_version = "#version 460";//exists in imguilayer
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);



        m_window = glfwCreateWindow(m_info.dimmensions.x, m_info.dimmensions.y, m_info.name.c_str(), nullptr, nullptr);
        if (!m_window)
            BREAK_POINT("Failed to create window");
        
        glfwMakeContextCurrent(m_window);
        int status  = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!status)
            BREAK_POINT("Failed to load gl loader");
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

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background
        style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.2f, 1.0f);   // Green button
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.7f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.4f, 0.1f, 1.0f);
        style.WindowRounding = 5.0f;              // Round window corners
        style.FramePadding = ImVec2(10.0f, 5.0f); // Padding within a frame
        style.ItemSpacing = ImVec2(8.0f, 4.0f);   // Spacing between items
        style.ScrollbarSize = 15.0f;              // Width of the scrollbar
        style.FrameRounding = 4.0f;               // Round frame corners
        //ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        //ImGui_ImplOpenGL3_Init(glsl_version);
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
