#pragma once
#include <unordered_map>
#include <span>
#include "Shader.hpp"
#include <boost/serialization/strong_typedef.hpp>

namespace Syris{
    class ShaderManager{
        public:
            using ShaderID = uint32_t;
            ShaderManager(){};
            ~ShaderManager(){};
            uint32_t add_shader(Shader::CreateInfo info);
            std::vector<uint32_t> add_shader(std::span<Shader::CreateInfo> shaders_info);
            void use_shader(ShaderID id);
            void use_shader(const char *name);
            Shader* get_shader(ShaderID id){return m_shaders[id];}
            ShaderID get_shader_id(const char* name);
        private:
            uint32_t m_id = 1;//0 is error
            std::unordered_map<ShaderID, Shader*> m_shaders;
            std::unordered_map<const char *, ShaderID> m_names_to_shaders;
    };
}