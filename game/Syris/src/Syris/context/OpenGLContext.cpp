#include "OpenGLcontext.hpp"
#include "Syris/shader/OpenGLShader.hpp"
#include "Syris/include/imgui.h"
#include "Syris/events/Event.hpp"
#include "Syris/utils/EngineTime.h"
#include "Syris/input/Input.h"
namespace Syris{
    OpenGLContext::OpenGLContext(GraphicsContext::CreateInfo &info) : m_layer_manager(), m_window(info.window_info)
    {

        m_program = glCreateProgram();
        setup_callbacks(m_window.get_window(),this);
        Input::get(m_window.get_window());
        //compile_shaders(m_program, info.vertex_shader_path,
         //                      info.fragment_shader_path);
    }
    bool OpenGLContext::should_window_close(){
        return m_window.should_window_close();
    }
    void OpenGLContext::on_update(Syris::engine_time::Time& time){
        //events
        glfwPollEvents();

        //set imgui frame
        int display_w, display_h;

        ImGui::SetCurrentContext(m_window.get_imgui_context());
        glfwGetFramebufferSize(m_window.get_window(), &display_w, &display_h);
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(display_w, display_h);

        //clear screen
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //render
        m_layer_manager.on_update(time);
        

        //resize
        glfwGetFramebufferSize(m_window.get_window(), &display_w, &display_h);

        //finish imgui frame
        io.DisplaySize = ImVec2(display_w, display_h);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);

        //swap buffer
        m_window.swap_buffers();
        
        //std::cout << "swap buffers\n";
    }
    bool OpenGLContext::on_event(Event* event){
        return m_layer_manager.on_event(event);
    }
}