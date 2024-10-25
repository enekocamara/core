#include "Statistics.hpp"
#include "Syris/include/imgui.h"
#include "Components.hpp"
#include "Syris/include/imgui.h"

namespace Syris{
    void render_root(entt::entity root, entt::registry& registry){
        ImGui::Text("Im Root");
        for (auto child : registry.get<statistics::CChilds>(root).childs){
            registry.get<statistics::CRender>(child).render(child, registry);
        }
    }

    Statistics::Statistics(){
        Statistics::AddModuleInfo info{
            .parent = std::nullopt,
            .render = render_root,
        };
        m_root_id = add_module(info);
        m_registry.emplace<statistics::CStatisticsRoot>(m_root_id);
    }
    void Statistics::on_update(engine_time::Time &time){
        ImGui::Begin("Statistics");
        m_registry.get<statistics::CRender>(m_root_id).render(m_root_id, m_registry);
        //render_module(m_root_id, true);
        /*
        if (ImGui::CollapsingHeader("Scenes"))
        {
            ImGui::SeparatorText("current scenes");
            auto scenes = m_registry.group<statistics::CScene>();
            int i = 0;
            static int selected = -1;
            int clicked_node = -1;
            ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            for (auto scene : scenes){
                ImGuiTreeNodeFlags node_flags = base_flags;
                if (selected == i)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, m_registry.get<statistics::CScene>(scene).scene_name.c_str());
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    clicked_node = i;
                if (node_open)
                {
                    
                    ImGui::TreePop();
                }
                i++;
            }
            if (clicked_node != -1)
                selected = clicked_node;
        }
        */
        ImGui::End();
    }
    void Statistics::render_module(StatisticModID id, bool recursive){
        /*uint64_t index = m_ids_x_index.from<StatisticModID>()[id];
        StatisticModule& mod = m_modules[index];
        for (auto& child : mod.get_childs()){
            render_module(child, true);
        }*/
    }
    StatisticModID Statistics::add_module(AddModuleInfo info)
    {
        entt::entity id = m_registry.create();
        m_registry.emplace<statistics::CRender>(id, info.render);
        m_registry.emplace<statistics::CChilds>(id);
        return id;
    }

    void Statistics::add_child(StatisticModID parent, StatisticModID child){
        m_registry.get<statistics::CChilds>(parent).childs.push_back(child);
    }
}