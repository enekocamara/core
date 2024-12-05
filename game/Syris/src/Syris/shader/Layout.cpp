#include "Layout.hpp"
#include <iostream>

#include "Syris/include/imgui.h"
namespace Syris{
    bool Layout::add_variable(std::string& name, Type type){
        m_variables[name] = type;
        m_order.push_back(name);
        return true;
    }
    bool Layout::add_variable(std::string_view name, Type type){
        m_variables[std::string(name)] = type;
        m_order.emplace_back(name);
        return true;
    }
    bool Layout::remove_variable(std::string& name){
        if (m_variables.find(name) == m_variables.end())
            return false;
        m_variables.erase(name);

        m_order.erase(std::find(m_order.begin(), m_order.end(), name));
        return true;
    }
    void Layout::render()const{
        ImGui::Text("Number of variables: %d", m_variables.size());
        if (ImGui::TreeNode("Varsiables:")){
            for (auto& [name, type] : m_variables){
                ImGui::Text(std::format("{} : {}", name, type::to_str(type)).c_str());
            }
            ImGui::TreePop();
        }
    }
    void Layout::debug()const{
        std::cout << "Number of variables: " << m_variables.size() << '\n';
        std::cout <<  "Varsiables:\n";
        for (auto &[name, type] : m_variables){
            std::cout << name << " : "  << type::to_str(type) << '\n';
        }
    }
}