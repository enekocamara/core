#include "ShaderManger.hpp"
#include <stdexcept>
#include "Syris/log/Log.h"
#include <format>
#include <iostream>
#include <expected>
namespace Syris{
    ShaderManager::ShaderManager(std::string path_start, Statistics& statistics): m_path_start(path_start){
        Statistics::AddModuleInfo mod_info{
            .render = std::bind(&ShaderManager::render_statictics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistics_mod_ID = statistics.add_module(mod_info);
        statistics.add_child(statistics.get_root(), m_statistics_mod_ID);
    }

    std::expected<ShaderManager::ShaderID, std::string> ShaderManager::add_shader(Shader::CreateInfo info){
        try{
            std::string real_path = std::format("{}\\{}", m_path_start, info.path);
            info.path = real_path.c_str();
            Shader* shader = Shader::create_shader(info);
            uint32_t id = m_id++;
            m_shaders[id] = shader;
            return id;
        } catch (std::runtime_error& e){
            CORE_ERROR(std::format("Error compiling shader, {}", e.what()));
            return std::unexpected(e.what());
        }
    }
    /*
    std::vector<uint32_t> ShaderManager::add_shader(std::span<Shader::CreateInfo> shaders_info){
        std::vector<uint32_t> results(shaders_info.size());
        int i = 0;
        for (auto& shader_info : shaders_info){
            results[i] = add_shader(shader_info);
            i++;
        }
        return results;
    }*/

    void ShaderManager::use_shader(ShaderID id, Uniform *uniforms){
        m_shaders[id]->use(uniforms);
    }

    void ShaderManager::use_shader(const char *name, Uniform *uniforms){
        m_shaders[m_names_to_shaders[name]]->use(uniforms);
    }
    uint32_t ShaderManager::get_shader_id(const char* name){
        if (m_names_to_shaders.find(name) != m_names_to_shaders.end()){
            return m_names_to_shaders[name];
        }
        return static_cast<ShaderID>(0);
    }
    void ShaderManager::on_update(const engine_time::Time& time){
        for (auto [key,shader] : m_shaders){
            shader->on_update(time);
        }
    }
    void ShaderManager::render_statictics(entt::entity entity, entt::registry& registry){
        ImGui::Text("Shader Manager");
    }
}