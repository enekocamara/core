#include "MaterialManager.hpp"
#include "Syris/statistics/Components.hpp"
#include "Syris/include/imgui.h"

namespace Syris{
    void MaterialManager::render(entt::entity entity, entt::registry& registry){
        ImGui::Text("Material Manager");
        ImGui::Text("Number of materials: %d", m_materials.size());
        if (m_materials.size() != 0){
            if (ImGui::TreeNode("Materials:")){
                for (auto child : registry.get<statistics::CChilds>(entity).childs){
                    if (ImGui::TreeNode(registry.get<statistics::CMaterial>(child).material_name.c_str())){
                        registry.get<statistics::CRender>(child).render(child, registry);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    MaterialManager::MaterialManager(CreateInfo info):m_statistics(info.statistics){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&MaterialManager::render, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CMaterialManager>(m_statistic_mod);
    }
    MaterialManager::~MaterialManager(){
        //todo
    }
    /*
    
    template<typename T>
    MaterialManager::MaterialID MaterialManager::add_material(Material::CreateInfo& info){
        Material* material = Material::create_material<T>(info);
        MaterialID id = m_current_id++;
        m_materials[id] = material;
        return id;
    }*/

    void MaterialManager::set_entity(MaterialID id, MaterialSetRequest& request){
        m_materials[id]->set_entity(request);
    }
    void MaterialManager::draw(MaterialID id, void *uniforms){
        m_materials[id]->draw(uniforms);
    }
    Material *MaterialManager::get_material(MaterialID id){
        return m_materials[id];
    }
}