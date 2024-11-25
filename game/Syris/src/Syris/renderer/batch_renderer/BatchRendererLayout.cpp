#include "BatchRendererLayout.hpp"
#include "Syris/statistics/Components.hpp"
namespace Syris{
    BatchRendererLayout::BatchRendererLayout(CreateInfo info)
        : m_attribute_layout(info.attributes_layout),
          m_uniforms_layout(info.uniforms_layout),
          m_statistics(info.statistics){ 
        std::cout << "DEBUG 2: number of variables" << m_attribute_layout.get_vars().size() << '\n';
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&BatchRendererLayout::render_statistics, this, std::placeholders::_1, std::placeholders::_2)};
        m_statistic_mod_ID = info.statistics.add_module(mod_info);
    }
    BatchRendererLayout::BatchRendererLayout(Shader* shader, Statistics& statistics)
        : m_attribute_layout(shader->get_attribute_layout()),
          m_uniforms_layout(shader->get_uniform_layout()),
          m_statistics(statistics){ 
        std::cout << "DEBUG: number of variables" << m_attribute_layout.get_vars().size() << '\n';
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&BatchRendererLayout::render_statistics, this, std::placeholders::_1, std::placeholders::_2)};
        m_statistic_mod_ID = statistics.add_module(mod_info);
    }
    std::expected<void, std::string> BatchRendererLayout::set_subbuffer(VertexBuffer::SubBufferInfo subbuffer){
        auto& layout_attributes = m_attribute_layout.get_vars();
        for (auto& attribute : subbuffer.get_attributes()){
            auto it = layout_attributes.find(attribute.name);
            if (it == layout_attributes.end())
                return std::unexpected(std::format("Attribute '{}' not found in shader attribute list.", attribute.name));
            else if (it->second != attribute.type)
                return std::unexpected(std::format("Attribute '{}' of type {} does not match shader attribute type '{}'",
                    attribute.name, type::to_str(attribute.type), type::to_str(it->second)));
            else if (m_attributes_set.contains(attribute.name))
                return std::unexpected(std::format("Attribute '{}' duplicated. It has already been set", attribute.name));
        }
        for (auto& attribute : subbuffer.get_attributes())
            m_attributes_set.insert(attribute.name);
        m_subbuffers.push_back(subbuffer);
    }
    std::expected<void, std::string> BatchRendererLayout::finish(){
        if (m_attributes_set.size() != m_attribute_layout.get_vars().size()){
            std::string unset_attributes;
            for(auto& [name, type] : m_attribute_layout.get_vars()){
                if (!m_attributes_set.contains(name))
                    unset_attributes += std::format("\tName: '{}' - Type: {};\n", name, type::to_str(type));
            }
            return std::unexpected(std::format("All attributes where not set in subbuffers: \nAttributes:\n{}", unset_attributes));
        }
    }

    void BatchRendererLayout::render_statistics(entt::entity entity, entt::registry& registry){
        ImGui::Text("Layout:");
        if (ImGui::TreeNode("Layout variables:")){
            m_attribute_layout.render();
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Uniform variables:")){
            m_uniforms_layout.render();
            ImGui::TreePop();
        }
    }
}