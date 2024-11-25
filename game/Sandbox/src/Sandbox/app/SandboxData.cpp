/*#include "SandboxData.hpp"
#include "../scene/TriangleScene.hpp"
#include "Syris/log/Log.h"
namespace Sandbox{
    SandboxData::SandboxData(CreateInfo info)
        : m_scene_manager({info.statistics}),
          m_graphics_context(info.graphics_context),
          m_statistics(info.statistics),
          m_gui_layer(),
          m_layer(new SandboxLayer(*this)){
        
        m_scene_simple = m_scene_manager.new_scene(std::make_unique<SimpleScene>(SimpleScene::CreateInfo{info.atlas_path, m_graphics_context, info.camera_info, info.statistics})),
        m_scene_triangle = m_scene_manager.new_scene(std::make_unique<TriangleScene>(TriangleScene::CreateInfo{m_registry, info.atlas_path, info.graphics_context, info.camera_info, info.statistics})),
        m_scene_sandbox = m_scene_manager.new_scene(std::make_unique<SandboxScene>(SandboxScene::CreateInfo{info.atlas_path, m_graphics_context->get, info.camera_info, info.statistics})),
        m_scene_manager.add_scene_to_statistics(std::dynamic_pointer_cast<SandboxScene>(m_scene_manager.get_scene(m_scene_sandbox))->get_statistic_mod_ID());
        m_current_scene_id = m_scene_sandbox;
        m_current_layer_id = -1;
        m_current_layer_id = m_graphics_context.get_layer_manager().push_layer(std::dynamic_pointer_cast<Syris::Layer>(m_scene_manager.get_scene(m_scene_sandbox)));
        //m_graphics_context.get_layer_manager().push_layer(std::dynamic_pointer_cast<Syris::Layer>(m_graphics_context.get_window_handler().get_layer()));
        //m_graphics_context.get_layer_manager().push_layer(m_gui_layer);
        m_graphics_context.get_layer_manager().push_layer(this);
        m_graphics_context.get_layer_manager().push_layer(static_cast<Syris::Layer *>(&info.statistics));

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
        ImGui::RadioButton("triangle", (int*)&m_next_scene, m_scene_triangle); ImGui::SameLine();
        ImGui::RadioButton("simple", (int*)&m_next_scene, m_scene_simple); ImGui::SameLine();
        ImGui::RadioButton("scene", (int*)&m_next_scene, m_scene_sandbox);
        ImGui::End();
    }
    void SandboxData::set_scene(Syris::SceneID scene){
        if (m_current_scene_id != scene){
            if (scene == m_scene_triangle)
                m_current_layer_id = m_graphics_context.get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_triangle)), m_current_layer_id);
            else if (scene == m_scene_simple)
                m_current_layer_id = m_graphics_context.get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_simple)), m_current_layer_id);
            else if (scene == m_scene_sandbox)
                m_current_layer_id = m_graphics_context.get_layer_manager().replace_layer(reinterpret_cast<Syris::Layer *>(m_scene_manager.get_scene(m_scene_sandbox)), m_current_layer_id);
            m_current_scene_id = scene;
        }
    }
}
*/
