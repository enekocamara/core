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
#include "Syris/texture/Texture.hpp"

#include "Sandbox/ecs/Components.h"
#include "Sandbox/scene/Sandbox/SandboxScene.hpp"
#include "Sandbox/scene/SimpleScene.hpp"
#include "Sandbox/scene/TriangleScene.hpp"
#include "Sandbox/texture/SandboxTexture.hpp"

#include "Syris/app/OpenGLWindow.hpp"
namespace Sandbox {
    SandboxApp::SandboxApp(SandboxApp::CreateInfo &info)
        : m_graphics_context(Syris::GraphicsContext::new_context(info.gc_info)),
          m_scene_manager({info.app_init.statistics}),
          m_statistics(info.app_init.statistics),
          m_dll_scripts("C:\\Users\\eneko\\dev\\asharis\\game\\build\\dll\\Debug", "Scripts")
    {
        /*
        const char* glsl_version = "#version 460";//exists in imguilayer
        if (!ImGui_ImplGlfw_InitForOpenGL(static_cast<Syris::OpenGLWindow*>(m_graphics_context->get_window_handler())->get_window(), true)){
            CORE_ERROR("failed to initialize ImGui_ImplGlfw for Opengl");
            exit(1);
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)){
            CORE_ERROR("failed to initialize ImGui_ImplOpengl3 with GLSL version");
            exit(1);
        }
        */
        // m_data(SandboxData::CreateInfo{m_registry, info.atlas_path, m_graphics_context, info.camera_info, info.app_init.statistics})
        // scene creation
        try{
            m_scene_simple = m_scene_manager.new_scene(std::make_unique<SimpleScene>(SimpleScene::CreateInfo{ info.atlas_path, m_graphics_context->get_shader_manager(), info.camera_info, info.app_init.statistics, m_dll_scripts.get_dll()}));
            m_scene_triangle = m_scene_manager.new_scene(std::make_unique<TriangleScene>(TriangleScene::CreateInfo{ info.atlas_path, m_graphics_context->get_shader_manager(), info.camera_info, info.app_init.statistics}));
            m_scene_sandbox = m_scene_manager.new_scene(std::make_unique<SandboxScene>(SandboxScene::CreateInfo{info.atlas_path, m_graphics_context->get_shader_manager(),info.camera_info, info.app_init.statistics,m_thread_pool, *m_graphics_context.get(), m_dll_scripts.get_dll()}));
        }catch(std::runtime_error& e){
            BREAK_POINT(std::format("Failed to create scene {}", e.what()));
        }
        // statistics
        m_scene_manager.add_scene_to_statistics(reinterpret_cast<SandboxScene *>(m_scene_manager.get_scene(m_scene_sandbox))->get_statistic_mod_ID());
        m_scene_manager.add_scene_to_statistics(reinterpret_cast<SimpleScene *>(m_scene_manager.get_scene(m_scene_simple))->get_statistics());

        // current scene and layer
        m_current_scene_id = m_scene_simple;
        m_current_layer_id = -1;
        m_current_layer_id = m_graphics_context->get_layer_manager().push_layer(m_scene_manager.get_scene(m_scene_simple));//m_graphics_context->get_layer_manager().push_layer(reinterpret_cast<Syris::Layer*>(m_scene_manager.get_scene(m_scene_sandbox)));
        m_next_scene = m_current_scene_id;
        // other layers
        m_graphics_context->get_layer_manager().push_layer(this);
        m_graphics_context->get_layer_manager().push_layer(&info.app_init.statistics);



    }

    SandboxApp::~SandboxApp(){} //{ delete quad; }

    inline std::string dummy(std::string text){
        return "dummy " + text;
    }
    void SandboxApp::run() {
        m_time.start();
        while (!m_graphics_context->should_window_close()) {
            m_time.next_frame();
            m_graphics_context->on_update(m_time);
            m_dll_scripts.check();
        }
        CORE_INFO("window closed\n");
    }
    bool SandboxApp::on_event(Syris::Event* event){ 
        return false;
    }
    void SandboxApp::on_update(const Syris::engine_time::Time& time){
        set_scene(m_next_scene);
        ImGui::ShowDemoWindow();
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scene")){
                ImGui::RadioButton("triangle", (int *)&m_next_scene, m_scene_triangle);
                ImGui::RadioButton("simple", (int *)&m_next_scene, m_scene_simple);
                ImGui::RadioButton("scene", (int *)&m_next_scene, m_scene_sandbox);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        /*
        ImGui::BeginTabBar("Tabs");
        if (ImGui::BeginTabItem("Scenes tab")){
            ImGui::Begin("SandboxData Settings");
            ImGui::RadioButton("triangle", (int *)&m_next_scene, m_scene_triangle);ImGui::SameLine();
            ImGui::RadioButton("simple", (int *)&m_next_scene, m_scene_simple);ImGui::SameLine();
            ImGui::RadioButton("scene", (int *)&m_next_scene, m_scene_sandbox);
            ImGui::End();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        */
    }
    void SandboxApp::set_scene(Syris::SceneID scene){
        if (m_current_scene_id != scene){
            if (scene == m_scene_triangle)
                m_current_layer_id = m_graphics_context->get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_triangle)), m_current_layer_id);
            else if (scene == m_scene_simple)
                m_current_layer_id = m_graphics_context->get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_simple)), m_current_layer_id);
            else if (scene == m_scene_sandbox)
                m_current_layer_id = m_graphics_context->get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_sandbox)), m_current_layer_id);
            m_current_scene_id = scene;
        }
    }
}

std::unique_ptr<Syris::Application> get_client_app(Syris::AppInit& app_init) {
    Syris::GraphicsContext::CreateInfo gc_info = {
        .layers_info = Syris::LayerManager::CreateInfo{
            .layers = {}},
        .window_info = Syris::Window::CreateInfo{
            .dimmensions = glm::ivec2(1920, 1080),
            .name = std::string("hola"),
            .vSync = true,
            .statistics = app_init.statistics
        },
        .statistics = app_init.statistics
    };

    float ratio = 1920.f / 1080.f;
    Sandbox::SandboxApp::CreateInfo app_info = Sandbox::SandboxApp::CreateInfo{
        .atlas_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\textures\\sprites\\atlas\\spritesheet.png",
        .gc_info = gc_info,
        .camera_info = Syris::OrthographicCameraController::CreateInfo{
            .aspec_ratio = ratio,
            .zoom_level = 10.f,
        },
        .app_init = app_init
    };
    return std::make_unique<Sandbox::SandboxApp>(app_info);
}
