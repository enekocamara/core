#include "App.h"

namespace app{
    App::App(Window::WindowBundle info) : m_window(info){
    }
    void App::run(){
        while(true){
            m_window.onUpdate();
        }
    }
}
