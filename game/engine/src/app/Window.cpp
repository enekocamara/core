#include "Window.h"

Window::Window(Window::WindowBundle info): m_info(info){
    init();
}
Window::~Window(){
    shutDown();
}
void Window::init(){
    int code = glfwInit();
    if (code != GLFW_TRUE){
        std::cerr << "Failed to init window\ncode : " << code << '\n';
        exit(1);
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    m_window = glfwCreateWindow(m_info.dimmensions.x, m_info.dimmensions.y, m_info.name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    int status  = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status){
        std::cerr << "Failed to init glad\n";
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

/*    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows*/
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
void Window::shutDown(){
    glfwDestroyWindow(m_window);
}
void Window::onUpdate(){
    glfwPollEvents();

    int display_w, display_h;
    ImGuiIO& io = ImGui::GetIO();
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    io.DisplaySize = ImVec2(display_w, display_h);

    ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window = true;

    ImGui::ShowDemoWindow(&show_demo_window);

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    glfwSwapBuffers(m_window);
}

