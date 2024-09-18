#include "ImGuiLayer.hpp"
#include "Syris/log/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace Sandbox{
    ImGuiLayer::ImGuiLayer(){
        /*m_gui_context = ImGui::CreateContext();
        const char* glsl_version = "#version 460";//exists in window
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
       */
    }
    ImGuiLayer::~ImGuiLayer(){

    }
    bool ImGuiLayer::on_event(Syris::Event* event){
        return false;
    }

    void ImGuiLayer::on_update(Syris::engine_time::Time& time){
       /*
        ImGui::SetCurrentContext(m_gui_context);
        //display size
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
        bool show_demo_window = false;
        ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        */
    }
}