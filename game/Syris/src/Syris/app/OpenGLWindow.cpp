#include "OpenGLWindow.h"
#include "Syris/log/Log.h"
#include "imgui.h"

namespace Syris{
    void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
            std::cerr << "OpenGL Debug Message: " << message << std::endl;
        if (severity == GL_DEBUG_SEVERITY_HIGH)
        {
            CORE_ERROR(message);
            throw std::runtime_error(message);
            exit(1);
        }
    }
    OpenGLWindow::OpenGLWindow(WindowCreateInfo& info): m_info(info){
        init();
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_window_layer = new OpenGLLayer(this);
    }
    OpenGLWindow::~OpenGLWindow(){
        shut_down();
        delete(m_window_layer);
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
        //glfwDestroyWindow(m_window);
    }
    void OpenGLWindow::on_update_start(){
        //glfwPollEvents();

        bool show_demo_window = false;

        ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        //ImGui::Render();

        //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLWindow::on_update_end(){
        /*
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
*/

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        //glfwSwapBuffers(m_window);
    }
    void OpenGLWindow::swap_buffers(){
        glfwSwapBuffers(m_window);
    }




    void OpenGLLayer::on_update(engine_time::Time& time){
        m_window->on_update_start();
        m_window->on_update_end();
    }
    bool OpenGLLayer::on_event(Event* event){
        return false;
    }

}
