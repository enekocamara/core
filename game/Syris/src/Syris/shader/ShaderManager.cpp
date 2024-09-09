#include "ShaderManger.hpp"
#include <stdexcept>
#include "Syris/log/Log.h"
namespace Syris{

    uint32_t ShaderManager::add_shader(Shader::CreateInfo info){
        
        try{
            Shader* shader = Shader::create_shader(info);
            auto path = std::string(info.fragment_path);
            std::string name = get_shader_name_from_path(path);
            uint32_t id = m_id++;
            m_shaders[id] = shader;
            return id;
        } catch (std::runtime_error& e){
            CORE_ERROR(std::format("Error compiling shader, {}", e.what()));
            return 0;
        }
    }
    std::vector<uint32_t> ShaderManager::add_shader(std::span<Shader::CreateInfo> shaders_info){
        std::vector<uint32_t> results(shaders_info.size());
        int i = 0;
        for (auto& shader_info : shaders_info){
            results[i] = add_shader(shader_info);
            i++;
        }
        return results;
    }

    void ShaderManager::use_shader(ShaderID id){
        m_shaders[id]->use();
    }

    void ShaderManager::use_shader(const char *name){
        m_shaders[m_names_to_shaders[name]]->use();
    }
    uint32_t ShaderManager::get_shader_id(const char* name){
        if (m_names_to_shaders.find(name) != m_names_to_shaders.end()){
            return m_names_to_shaders[name];
        }
        return static_cast<ShaderID>(0);
    }
}