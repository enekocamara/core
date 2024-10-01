#pragma once
#include <unordered_map>
#include <span>
#include "Shader.hpp"
#include <boost/serialization/strong_typedef.hpp>

namespace Syris{
    class ShaderManager{
        public:
            using ShaderID = uint32_t;
            ShaderManager(std::string path_start) : m_path_start(path_start){};
            ~ShaderManager(){};
            uint32_t add_shader(Shader::CreateInfo info);
            std::vector<uint32_t> add_shader(std::span<Shader::CreateInfo> shaders_info);
            void use_shader(ShaderID id, void * uniforms);
            void use_shader(const char *name, void *uniforms);
            Shader* get_shader(ShaderID id){return m_shaders[id];}
            ShaderID get_shader_id(const char* name);
        private:
            uint32_t m_id = 1;//0 is error
            std::unordered_map<ShaderID, Shader*> m_shaders;
            std::unordered_map<const char *, ShaderID> m_names_to_shaders;
            std::string m_path_start;
    };
}