#pragma once
#include <string>
#include <glm/glm.hpp>
#include <span>
#include <tuple>
#include "Syris/layers/Layer.hpp"

namespace Syris{
    std::string get_shader_name_from_path(std::string& path);
/*
    class IShaderLayout{
        public:
            IShaderLayout(){}
            ~IShaderLayout(){}
            virtual void set(void *data) = 0;
            virtual void* get() = 0; 
    };

    template<typename ...Args>
    class ShaderLayout : public IShaderLayout{
        public:
            ShaderLayout(std::span<std::string> names);
            ~ShaderLayout();
        //types strings
        private:
            std::tuple<Args...> values;
            std::unordered_map<uint32_t, std::string> var_name;
    };
*/
    class Shader : public Layer{
        public:
            struct CreateInfo{
                const char *fragment_path; 
                const char *vertex_path; 
            };
            //Shader(){};
            virtual ~Shader(){};
            static Shader* create_shader(CreateInfo info);
            virtual void use() = 0;
            virtual void set_uniform_value(glm::mat4, const char* name) = 0;
            virtual void set_uniform(glm::vec3 val, const char *name) = 0;
            virtual void set_uniform1i(int texture_index, const char* name) = 0;
//            virtual void on_update(engine_time::Time)override;
            //virtual IShaderLayout& get_shader_layout() = 0;
    };
}