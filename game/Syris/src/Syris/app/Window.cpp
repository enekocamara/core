#include "Window.h"
#include "Syris/log/Log.h"
#include "imgui.h"

namespace Syris{
    void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        if (severity == GL_DEBUG_SEVERITY_HIGH)
        {
            CORE_ERROR(message);
            throw std::runtime_error(message);
            exit(1);
        }
    }
    Window::Window(Window::WindowBundle info): m_info(info){
        init();
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    Window::~Window(){
        shutDown();
    }
    void Window::init(){
        int code = glfwInit();
        if (code != GLFW_TRUE){
            CORE_ERROR(std::format("Failed to init window: error code: {}", code));
            exit(1);
        }

        const char* glsl_version = "#version 460";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

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
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)){
            CORE_ERROR("failed to initialize ImGui_ImplGlfw for Opengl");
            exit(1);
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)){
            CORE_ERROR("failed to initialize ImGui_ImplOpengl3 with GLSL version");
            exit(1);
        }
    }
    void Window::shutDown(){
        //glfwDestroyWindow(m_window);
    }
    void Window::onUpdateStart(){
        glfwPollEvents();

        int display_w, display_h;

        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(display_w, display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");
        ImGui::Button("hello");
        float value = 1.f;
        ImGui::DragFloat("value", &value);
        ImGui::End();
//        bool show_demo_window = false;

//        ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();

        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Window::onUpdateEnd(){
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        if (!backup_current_context){
            CORE_ERROR("failed to back up glfw current context");
            exit(1);
        }
        glfwMakeContextCurrent(backup_current_context);
        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        glfwSwapBuffers(m_window);
    }
}
