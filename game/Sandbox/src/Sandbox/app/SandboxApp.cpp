#pragma once
#include <array>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Sandbox/app/SandboxApp.hpp"

#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/renderer/AttributeLayoutList.hpp"
#include "Syris/input/Input.h"
#include "Syris/Shader/Shader.hpp"
#include "Syris/renderer/Texture.h"

#include "Sandbox/ecs/Components.h"
#include "Sandbox/scene/Sandbox/SandboxScene.hpp"
#include "Sandbox/texture/SandboxTexture.hpp"

namespace Sandbox {
       SandboxApp::SandboxApp(SandboxApp::CreateInfo& info)
        : 
        m_graphics_context(info.gc_info),
        m_data(SandboxData::CreateInfo{m_registry, info.atlas_path, m_graphics_context, info.camera_info})
        { 
            // m_window(m_graphics_context.get_window_handler()),
            // m_camera(info.cameraCreateInfo)
            
            //init program and shaders
            /*now in the graphics context, shader manager
            m_program = glCreateProgram();
            shader::compile_shader(m_program, info.vertex_shader_path,
                    info.fragment_shader_path);
            */
            //textures, timestamp, and input initialization
            m_time.init_time = std::chrono::high_resolution_clock::now();
            //Syris::Input::get(m_window.get_window()); done by graphics context

//            m_renderBuffer = makeSimpleRenderBuffer(); 
            //render objects
       //     quad = makeQuad();
            
         /*   //we will create a render buffer for the tileset
            Syris::renderer::RenderBuffer::CreateInfo render_buffer_info = {};
            render_buffer_info.dynamic = true; 
            using AttributeLayout = Syris::VertexBuffer::AttributeLayout;
            std::array<AttributeLayout, 4> layouts;
            uint32_t stride_count = 2;
            AttributeLayout pos_layout = {};
            pos_layout.index_layout = 0;
            pos_layout.normalize = false;
            pos_layout.values_count = 2;
            pos_layout.skip_count = nullptr;
            pos_layout.stride_count = stride_count; 
           */ 
        }

    SandboxApp::~SandboxApp(){} //{ delete quad; }

    void SandboxApp::run() {
        m_time.last_frame = std::chrono::high_resolution_clock::now();
        while (!m_graphics_context.should_window_close()) {
            CHECK_GL_ERROR();
            std::chrono::high_resolution_clock::time_point now =
                std::chrono::high_resolution_clock::now();
            m_time.delta_time_ms =
                std::chrono::duration<float, std::milli>(now - m_time.last_frame)
                .count();
            m_graphics_context.on_update(m_time);
            m_time.last_frame = now;
            CHECK_GL_ERROR();
        }
        std::cout << "window closed\n";
    }
/*DEPRECATED
    void SandboxApp::draw_frame() {

        glUseProgram(m_program);
        static_cast<SandboxScene*>(m_scene)->onUpdate(m_program, m_camera.getCamera().get_view_projection_matrix());
//        m_renderBuffer->draw_buffer(m_program, m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture());
        //auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
        //        ecs::CPosition, ecs::CTile>) ;

        //draw_group(render_group);
    }
    */

    template <typename Group> void SandboxApp::draw_group(Group &group) {
 /*       float ratio = 1920.f / 1080.f;
        for (auto entity : group) {
            auto [cTexture, cPosition] =
                group.get<ecs::CTexture, ecs::CPosition>(entity);
            Syris::texture::Texture2D texture = m_texture_atlas.getTexture();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                    model, glm::vec3(cPosition.pos,
                        0.0f)); // first translate (transformations are: scale
                                // happens first, then rotation, and then final
                                // translation happens; reversed order)

            
            //   model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y,
             //  0.0f)); // move origin of rotation to center of quad model =
             //  glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); //
             //  then rotate model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f
            //  size.y, 0.0f)); // move origin back
            
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f)); // last scale

            CHECK_GL_ERROR();

            m_renderer.draw_quad(m_program, quad, model,
                    m_camera.getCamera().get_view_projection_matrix(),texture, cTexture.rect
                    ,glm::vec3(1.f));
        }
    */
        // exit(1);
    }

    //main layer
} // namespace Sandbox

Application* get_client_app() {
    Syris::GraphicsContext::CreateInfo gc_info = {
        .layers_info = Syris::LayerManager::CreateInfo{
            .layers = {}},
        .window_info = Syris::WindowCreateInfo{
            .dimmensions = glm::ivec2(1920, 1080),
            .name = std::string("hola"),
            .vSync = false,
        }
    };
    //Syris::Window::WindowBundle info =   
    float ratio = 1920.f / 1080.f;
    Sandbox::SandboxApp::CreateInfo app_info = Sandbox::SandboxApp::CreateInfo{
        .atlas_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\textures\\sprites\\atlas\\spritesheet.png",
        .gc_info = Syris::GraphicsContext::CreateInfo{
            .layers_info = Syris::LayerManager::CreateInfo{
                .layers = {}},
            .window_info = Syris::WindowCreateInfo{
                .dimmensions = glm::ivec2(1920, 1080),
                .name = std::string("hola"),
                .vSync = true,
            },
            //.vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\vertexShader.glsl",
            //.fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\fragmentShader.glsl",
        },
        .camera_info = Syris::OrthographicCameraController::CreateInfo{
            .aspec_ratio = ratio,
            .zoom_level = 10.f,
        }};
        //  .window_bundle = info,
        //.cameraCreateInfo = Syris::CameraOrthographic::CreateInfo{
        //    .left = -1.0f,
        //    .right = 1.0f,
        //    .top  = 1.0f,
        //    .bottom = -1.0f
        //}
        return new Sandbox::SandboxApp(app_info);
}
