#include "SceneManager.hpp"
#include "Syris/statistics/Components.hpp"
#include "Syris/include/imgui.h"

namespace Syris{
    void SceneManager::render_statistics(StatisticModID id, entt::registry& registry){
        ImGui::Text("Scene Manager");
        int i = 0;
        auto& childs = registry.get<statistics::CChilds>(m_statistics_mod_ID).childs;
        for (auto child : childs){
            if (ImGui::TreeNode(registry.get<statistics::CScene>(child).scene_name.c_str())){
                registry.get<statistics::CRender>(child).render(child, registry);
                ImGui::TreePop();
            }
            i++;
        }
    }

    SceneManager::SceneManager(CreateInfo info): m_statistics(info.statistics){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&SceneManager::render_statistics, this, std::placeholders::_1, std::placeholders::_2),
        };
        m_statistics_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CSceneManager>(m_statistics_mod_ID);
        info.statistics.add_child(info.statistics.get_root(), m_statistics_mod_ID);
    }
    SceneManager::~SceneManager(){
    }
    SceneID SceneManager::new_scene(std::unique_ptr<Scene> scene){
        SceneID id = m_current_scene_ID++; 
        m_scenes[id] = std::move(scene);
        return id;
    }
    void SceneManager::add_scene_to_statistics(StatisticModID id){
        m_statistics.add_child(m_statistics_mod_ID, id);
    }
}