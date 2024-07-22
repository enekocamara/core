#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Syris/log/Log.h"
#include "Syris/input/KeyCodes.h"

namespace Syris {
    class Input{
        public:
            Input() = delete;
            Input(const Input& ref) = delete;
            Input& operator=(const Input& ref) = delete;

            static Input& get(GLFWwindow* window = nullptr){
                static Input input(window);
                return input;
            }
            static bool is_key_pressed(KeyCode key_code){
                int state = glfwGetKey(get().m_current_window,static_cast<int>(key_code));
                return state == GLFW_PRESS || state == GLFW_REPEAT;
            }
        private:
            Input(GLFWwindow* window){
                if (window == nullptr){
                    Logger::core_error("Input constructor called with nullptr window parameter");
                    throw std::runtime_error("Input constructor called with nullptr window parameter");
                }
                m_initialized = true;
                m_current_window = window;
            }
            bool m_initialized = false;
            GLFWwindow* m_current_window;        
    };
}
