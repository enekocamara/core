#include "SandboxData.hpp"
#include "../scene/TriangleScene.hpp"
#include "Syris/log/Log.h"
namespace Sandbox{
    SandboxData::SandboxData(CreateInfo info)
        : m_registry(info.registry),
          m_scene_simple(new SimpleScene({m_registry, info.atlas_path, info.graphics_context, info.camera_info})),
          m_scene_triangle(new TriangleScene({m_registry, info.atlas_path, info.graphics_context, info.camera_info})),
          m_scene_sandbox(new SandboxScene({m_registry, info.atlas_path, info.graphics_context, info.camera_info})),
          m_graphics_context(info.graphics_context),
          m_gui_layer(),
          m_layer(new SandboxLayer(*this)){
        m_current_scene_id = m_graphics_context.get_layer_manager().push_layer(m_scene_triangle);
        m_graphics_context.get_layer_manager().push_layer((Syris::Layer*)m_graphics_context.get_window_handler().get_layer());
        m_graphics_context.get_layer_manager().push_layer(&m_gui_layer);
        m_graphics_context.get_layer_manager().push_layer(this);

        const char* glsl_version = "#version 460";//exists in imguilayer
        if (!ImGui_ImplGlfw_InitForOpenGL(m_graphics_context.get_window(), true)){
            CORE_ERROR("failed to initialize ImGui_ImplGlfw for Opengl");
            exit(1);
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)){
            CORE_ERROR("failed to initialize ImGui_ImplOpengl3 with GLSL version");
            exit(1);
        }
    }
    SandboxData::~SandboxData(){

    }
    bool SandboxData::on_event(Syris::Event* event){
        return false;
    }
    void SandboxData::on_update(Syris::engine_time::Time& time){
        set_scene(m_next_scene);
        ImGui::Begin("SandboxData Settings"); 
        ImGui::RadioButton("triangle", (int*)&m_next_scene, (int)Scenes::Triangle); ImGui::SameLine();
        ImGui::RadioButton("simple", (int*)&m_next_scene, (int)Scenes::Simple); ImGui::SameLine();
        ImGui::RadioButton("scene", (int*)&m_next_scene, (int)Scenes::Sandbox);
        ImGui::End();
    }
    void SandboxData::set_scene(Scenes scene){
        if (m_current_scene != scene){
            switch(scene){
                case Scenes::Triangle:
                    m_current_scene_id = m_graphics_context.get_layer_manager().replace_layer(m_scene_triangle, m_current_scene_id);
                    break;
                case Scenes::Simple:
                    m_current_scene_id = m_graphics_context.get_layer_manager().replace_layer(m_scene_simple, m_current_scene_id);
                    break;
                case Scenes::Sandbox :
                    m_current_scene_id = m_graphics_context.get_layer_manager().replace_layer(m_scene_sandbox, m_current_scene_id);
                    break;
            }
            m_current_scene = scene;
        }
    }
}
