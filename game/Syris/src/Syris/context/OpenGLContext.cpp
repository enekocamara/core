#include "OpenGLcontext.hpp"
#include "Syris/shader/OpenGLShader.hpp"
#include "Syris/include/imgui.h"
#include "Syris/events/Event.hpp"
#include "Syris/utils/EngineTime.hpp"
#include "Syris/input/Input.h"
#include "Syris/renderer/Renderer.hpp"

namespace Syris{
    inline void ShowMainDockSpace()
    {
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

        // Set up a full-screen window for the dock space
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("MainDockSpace", &dockSpaceOpen, windowFlags);
        ImGui::PopStyleVar(2);

        // Create the dock space
        ImGuiID dockSpaceID = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
        ImGui::End();
    }

    OpenGLContext::OpenGLContext(GraphicsContext::CreateInfo &info)
        : m_shader_manager(std::string("C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders"), info.statistics),
          m_layer_manager(),
          m_window(std::make_unique<OpenGLWindow>(info.window_info)),
          m_render_framebuffer(info.window_info.dimmensions),
          m_main_framebuffer(0, {1920,1080}),
          m_fps("Render")
    {
        
        m_program = glCreateProgram();
        setup_callbacks(m_window->get_window(),this);
        Input::get(m_window->get_window());
        
        const char* glsl_version = "#version 460";//exists in imguilayer
        if (!ImGui_ImplGlfw_InitForOpenGL(m_window->get_window(), true)){
            CORE_ERROR("failed to initialize ImGui_ImplGlfw for Opengl");
            exit(1);
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)){
            CORE_ERROR("failed to initialize ImGui_ImplOpengl3 with GLSL version");
            exit(1);
        }

        Renderer::push_framebuffer(&m_main_framebuffer);
        Renderer::push_viewport({1920, 1080});
    }
    bool OpenGLContext::should_window_close(){
        return m_window->should_window_close();
    }
    void OpenGLContext::on_update(const Syris::engine_time::Time& time){
        //events
        m_shader_manager.on_update(time);
        glfwPollEvents();


        m_fps.next_frame(time);

        //set imgui frame

        //clear screen
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowMainDockSpace();

        //render image

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec2 oldPadding = style.WindowPadding;
        float oldRounding = style.WindowRounding;
        ImVec4 oldBgColor = style.Colors[ImGuiCol_TitleBg];
        ImVec4 oldBgActiveColor = style.Colors[ImGuiCol_TitleBgActive];

        // Customize style for a minimal window
        style.WindowPadding = ImVec2(0, 0);                        // Remove padding
        style.WindowRounding = 0.0f;                               // Remove corner rounding
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 0);       // Make title bar background invisible
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 0); // Invisible when active


        Renderer::push_framebuffer(&m_render_framebuffer);
        glViewport(0, 0, m_render_framebuffer.get_size().x, m_render_framebuffer.get_size().y); // Match the size of the framebuffer
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_layer_manager.on_update(time);

        ImGui::SetNextWindowBgAlpha(0.0f); // Make the background fully transparent (optional)
        ImGui::Begin("render", nullptr, 0);


        ImVec2 window_size = ImGui::GetContentRegionAvail();                              // Get available size in the window        
        m_current_window_size = {window_size.x, window_size.y};
        ImGui::Image((void *)(intptr_t)m_render_framebuffer.get_texture_id(), window_size, ImVec2(0, 1), ImVec2(1, 0)); // Display the texture

        ImGui::End();
        Renderer::pop_framebuffer();

        style.WindowPadding = oldPadding;
        style.WindowRounding = oldRounding;
        style.Colors[ImGuiCol_TitleBg] = oldBgColor;
        style.Colors[ImGuiCol_TitleBgActive] = oldBgActiveColor;

        m_fps.render_frame_count();

        ImGui::Render();


        int display_w, display_h;
        ImGui::SetCurrentContext(m_window->get_imgui_context());
        glfwGetFramebufferSize(m_window->get_window(), &display_w, &display_h);
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(display_w, display_h);
        glViewport(0, 0, display_w, display_h);
        if (!(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
            BREAK_POINT("View ports not enabled");
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        //swap buffer
        m_window->swap_buffers();
        
    }
    bool OpenGLContext::on_event(Event* event){
        return m_layer_manager.on_event(event);
    }
}