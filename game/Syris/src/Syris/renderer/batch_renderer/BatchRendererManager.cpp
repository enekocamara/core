#include "BatchRendererManager.hpp"
#include "Syris/statistics/Components.hpp"
#include "Syris/include/imgui.h"

namespace Syris{
     
    BatchRendererManager::BatchRendererManager(CreateInfo info):m_statistics(info.statistics){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&BatchRendererManager::render, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CBatchRendererManager>(m_statistic_mod);
    }
    BatchRendererManager::~BatchRendererManager(){
    }
    
    void BatchRendererManager::set_entity(BR_ID id,BR_SetRequest& request){
        m_batch_renderers[id]->set_entity(request);
    }
    void BatchRendererManager::draw(BR_ID id, Uniform *uniforms){
        m_batch_renderers[id]->draw(uniforms);
    }
    BatchRenderer* BatchRendererManager::get_renderer(BR_ID id){
        return m_batch_renderers[id].get();
    }

    void BatchRendererManager::render(entt::entity entity, entt::registry& registry){
        ImGui::Text("Batch renderer Manager");
        ImGui::Text("Number of materials: %d", m_batch_renderers.size());
        if (m_batch_renderers.size() != 0){
            if (ImGui::TreeNode("Materials:")){
                for (auto child : registry.get<statistics::CChilds>(entity).childs){
                    if (ImGui::TreeNode(registry.get<statistics::CBatchRenderer>(child).material_name.c_str())){
                        registry.get<statistics::CRender>(child).render(child, registry);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
}