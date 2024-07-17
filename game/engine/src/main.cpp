// ex0.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//#include "Renderer.h"
#include "App/App.h"

int main()
{
    Window::WindowBundle info = Window::WindowBundle{
        .dimmensions = glm::ivec2(1920, 1080),
        .name = std::string("hola"),
        .vSync = true,
    };
    app::App app(info);
    app.run();
 //   ge::Renderer renderer = ge::Renderer();
 //   renderer.run();
    return 0;
}

