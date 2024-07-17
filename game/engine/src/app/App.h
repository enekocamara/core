#pragma once
#include "Window.h"
namespace app{
    class App{
        public:
            App(Window::WindowBundle info);
            void run(); 

        private:
            Window m_window;
    };
}
