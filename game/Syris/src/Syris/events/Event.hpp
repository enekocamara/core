#pragma once
namespace Syris{

    enum class EventType{
        None = 0,
		WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,
		KeyPressed,
        KeyReleased,
        KeyTyped,
		MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    class Event{
        public:
            Event(){};
            virtual ~Event(){};
            virtual EventType get_type()const = 0;
            virtual bool has_been_handled() const = 0;
    };
    class EventKey : public Event{
        public:
            EventKey( int key, int scancode, int action, int mods);
            ~EventKey(){};
            EventType get_type()const override;
            int get_key()const{return m_key;}
            bool has_been_handled() const override{return m_has_been_handled;}
        private:
            int m_key;
            int m_scancode;
            int m_action;
            int m_mods;
            bool m_has_been_handled = false;
    };
    class EventMouse : public Event{
        public:
            EventMouse(int button, int action, int mods);
            ~EventMouse(){};
            EventType get_type()const override;
            bool has_been_handled() const override{return m_has_been_handled;}
        private:
            int m_button;
            int m_action;
            int m_mods;
            bool m_has_been_handled = false;
    };
    class EventCursorPos : public Event{
        public:
            EventCursorPos( double xpos, double ypos);
            ~EventCursorPos(){};

            EventType get_type()const override;
            bool has_been_handled() const override{return m_has_been_handled;}
        private:
            double m_xpos;
            double m_ypos;
            bool m_has_been_handled = false;
   };
    class EventScroll : public Event{
        public:
            EventScroll( double xoffset, double yoffset);
            ~EventScroll(){};
            EventType get_type()const override;
            bool has_been_handled() const override{return m_has_been_handled;}

        private:
            double m_xoffset;
            double m_yoffset;
            bool m_has_been_handled = false;
    };
    class EventWindowResize : public Event{
        public:
            EventWindowResize(int width, int height);
            ~EventWindowResize(){};
            EventType get_type()const override;
            bool has_been_handled() const override{return m_has_been_handled;}
            private: 
                int m_width;
                int m_height;
                bool m_has_been_handled = false;
    };

}


#ifdef RENDER_API_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "Syris/context/OpenGLContext.hpp"
namespace Syris{
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void setup_callbacks(GLFWwindow *window, void* context);
}
#endif