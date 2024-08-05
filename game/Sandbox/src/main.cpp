#include "app/SandboxApp.hpp"
#include "texture/SandboxTexture.hpp"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"

int main(){
    //Syris::Logger::client_trace(std::format("{},{}", Sandbox::texture::atlas::grass_0.min, Sandbox::texture::atlas::grass_0.max).c_str());
    Syris::Window::WindowBundle info = Syris::Window::WindowBundle{
        .dimmensions = glm::ivec2(1920, 1080),
        .name = std::string("hola"),
        .vSync = true,
    };
    entt::registry registry;
    float ratio = 1920.f / 1080.f;
    Sandbox::SandboxApp::CreateInfo app_info = Sandbox::SandboxApp::CreateInfo{
        .vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl",
        .fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl",
        .atlas_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\textures\\sprites\\atlas\\spritesheet.png",
        .window_bundle = info,
        .registry = registry,
        /*.cameraCreateInfo = Syris::CameraOrthographic::CreateInfo{
            .left = -1.0f,
            .right = 1.0f,
            .top  = 1.0f,
            .bottom = -1.0f

        }*/
        .cameraCreateInfo = Syris::OrthographicCameraController::CreateInfo{
            .aspec_ratio = ratio,
            .zoom_level = 10.f,
        }
    };

    Sandbox::SandboxApp app(app_info);
    CHECK_GL_ERROR();
    app.run();
    return 0;
}
