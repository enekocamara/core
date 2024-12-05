#pragma once
#include <unordered_map>
#include <span>
#include <expected>
#include "Shader.hpp"
//#include <boost/serialization/strong_typedef.hpp>
#include "Syris/statistics/Statistics.hpp"

namespace Syris{
    class ShaderManager{
        public:
            using ShaderID = uint32_t;
            ShaderManager(std::string path_start, Statistics& statistics);
            ~ShaderManager(){};
            std::expected<ShaderManager::ShaderID, std::string> add_shader(Shader::CreateInfo info);
            //std::vector<uint32_t> add_shader(std::span<Shader::CreateInfo> shaders_info);
            void use_shader(ShaderID id, Uniform *uniforms);
            void use_shader(const char *name, Uniform *uniforms);
            Shader* get_shader(ShaderID id){return m_shaders[id];}
            ShaderID get_shader_id(const char* name);
            void on_update(const engine_time::Time& time);
            void render_statictics(entt::entity entity, entt::registry& registry);
        private:
            uint32_t m_id = 1;//0 is error
            StatisticModID m_statistics_mod_ID;
            std::unordered_map<ShaderID, Shader*> m_shaders;
            std::unordered_map<const char *, ShaderID> m_names_to_shaders;
            std::string m_path_start;
    };
}