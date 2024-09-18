#include "Event.hpp"
#ifdef RENDER_API_OPENGL
#include "Syris/context/OpenGLContext.hpp"
namespace Syris{
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
        OpenGLContext *context = reinterpret_cast<OpenGLContext *>(glfwGetWindowUserPointer(window));
        EventKey key_event(key, scancode, action, mods);
        context->on_event(static_cast<Event*>(&key_event));
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            return;
        }
        // Pass the event to ImGui if it wasn't handled
        //ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        OpenGLContext *context = reinterpret_cast<OpenGLContext *>(glfwGetWindowUserPointer(window));
        EventMouse mouse_event(button, action, mods);
        context->on_event(static_cast<Event*>(&mouse_event));
        // Pass the event to ImGui if it wasn't handled
//        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }

    void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
    {

        OpenGLContext *context = reinterpret_cast<OpenGLContext *>(glfwGetWindowUserPointer(window));
        EventCursorPos cursor_pos_event(xpos,ypos);
        context->on_event(static_cast<Event*>(&cursor_pos_event));
        // Pass the event to ImGui if it wasn't handled
//        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {

        OpenGLContext *context = reinterpret_cast<OpenGLContext *>(glfwGetWindowUserPointer(window));
        EventScroll scroll_event(xoffset, yoffset);
        context->on_event(static_cast<Event*>(&scroll_event));
        // Pass the event to ImGui if it wasn't handled
//        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        OpenGLContext *context = reinterpret_cast<OpenGLContext *>(glfwGetWindowUserPointer(window));
        EventWindowResize window_resize_event(width, height);
        context->on_event(static_cast<Event*>(&window_resize_event));
    }

    void setup_callbacks(GLFWwindow *window, void* context)
    {
        glfwSetWindowUserPointer(window, context);
        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }
    //defien 4 callback functions
}
#endif

namespace Syris{

    //key
    EventKey::EventKey(int key, int scancode, int action, int mods){
    }
    EventType EventKey::get_type() const {
        return EventType::KeyPressed;
    }

    //mouse
    EventMouse::EventMouse(int button, int action, int mods):m_button(button),m_action(action),m_mods(mods){

    }
    EventType EventMouse::get_type() const{
        return EventType::MouseButtonPressed;//!todo not correct type
    }


    //cursor
    EventCursorPos::EventCursorPos(double xpos, double ypos):m_xpos(xpos), m_ypos(ypos){

    }
    EventType EventCursorPos::get_type() const{
        return EventType::MouseMoved;
    }

    //scroll
    EventScroll::EventScroll(double xoffset, double yoffset) : m_xoffset(xoffset), m_yoffset(yoffset)
    {
    }
    EventType EventScroll::get_type() const
    {
        return EventType::MouseScrolled;
    }

    //window resize
    EventWindowResize::EventWindowResize(int width, int height): m_width(width), m_height(height)
    {
    }
    EventType EventWindowResize::get_type() const
    {
        return EventType::WindowResize;
    }
}