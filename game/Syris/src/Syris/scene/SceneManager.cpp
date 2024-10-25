#include "SceneManager.hpp"
#include "Syris/statistics/Components.hpp"
#include "Syris/include/imgui.h"

namespace Syris{
    void render_scene_manager(StatisticModID id, entt::registry& registry){
        ImGui::Text("Scene Manager");
        int i = 0;
        static int selected = -1;
        int clicked_node = -1;

        ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        
        for (entt::entity child : registry.get<statistics::CChilds>(id).childs)
        {
           // registry.get<statistics::CRender>(child).render(child);
            ImGuiTreeNodeFlags node_flags = base_flags;
            if (selected == i)
                node_flags |= ImGuiTreeNodeFlags_Selected;
            bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags, registry.get<statistics::CScene>(child).scene_name.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                clicked_node = i;
            if (node_open)
            {
                registry.get<statistics::CRender>(child).render(child, registry);
                ImGui::TreePop();
            }
            i++;
        }
        if (clicked_node != -1)
            selected = clicked_node;
    }

    SceneManager::SceneManager(CreateInfo info): m_statistics(info.statistics){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = render_scene_manager,
        };
        m_statistics_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CSceneManager>(m_statistics_mod_ID);
        info.statistics.add_child(info.statistics.get_root(), m_statistics_mod_ID);
    }
    SceneManager::~SceneManager(){
        for (auto [key, scene] : m_scenes){
            delete scene;
        }
    }
    SceneID SceneManager::new_scene(Scene* scene){
        SceneID id = m_current_scene_ID++; 
        m_scenes[id] = scene;
        return id;
    }
    void SceneManager::add_scene_to_statistics(StatisticModID id){
        m_statistics.add_child(m_statistics_mod_ID, id);
    }
}