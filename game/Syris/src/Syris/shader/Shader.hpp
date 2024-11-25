#pragma once
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/int.hpp>

#include <string>
#include <glm/glm.hpp>
#include <span>
#include <tuple>
#include <vector>
#include <iostream>

#include "Syris/layers/Layer.hpp"
#include "Syris/shader/Layout.hpp"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"

namespace Syris{
    std::string get_shader_name_from_path(std::string& path);

    class IShaderLayout{
        public:
            IShaderLayout(){}
            ~IShaderLayout(){}
            virtual void set(void *data) = 0;
            virtual void* get() = 0; 
            virtual void check_layout(int program) = 0;
    };

    template<typename ...Args>
    class ShaderLayout : public IShaderLayout{
        using TupleType = std::tuple<Args...>;
        public:
            ShaderLayout(std::span<std::string> names){
                m_var_name = std::vector<std::string>(names.begin(), names.end());
            }
            ~ShaderLayout() {}

            template <std::size_t... I, typename TupleType>
            void iterate_tuple(std::index_sequence<I...>, TupleType &tuple, int program)
            {
                // Expand the parameter pack and call process for each element in the tuple
                (process(std::get<I>(tuple), I, program), ...);
            }

            void set(void *data){
                std::tuple<int, void *>* hold = reinterpret_cast<std::tuple<int, void*>*>(data);
                int program = std::get<0>(*hold);
                TupleType* values = reinterpret_cast<TupleType*>(std::get<1>(*hold));
                iterate_tuple(std::make_index_sequence<std::tuple_size<TupleType>::value>{}, *values, program);
                //todo
            }
            void* get(){
                //todo
                return nullptr;
            }
            void check_layout(int program){
                for (auto& name : m_var_name){
                    GLint location = glGetUniformLocation(program, name.c_str());
                    if (location == -1)
                    {
                        CORE_ERROR(std::format("Uniform {} not found in shader program", name));
                        exit(1);
                    }
                }
            }
        //types strings
        private:
            template<typename T>
            void process(T& data, std::size_t index, int program){
                const char * name = m_var_name[index].c_str();
                if constexpr (std::is_same_v<T, float>)
                {
                    //std::cout << "Processing float: " << *data << std::endl;
                    //renderAPI::set_uniform_value(program, data, name);
                    exit(1);
                }
                else if constexpr (std::is_same_v<T, glm::mat4>)
                {
                    //std::cout << "Processing glm::mat4" << std::endl;
                    renderAPI::set_uniform_value(program, data, name);
                }
                else if constexpr (std::is_same_v<T, glm::vec3>)
                {
                    //std::cout << "Processing glm::vec3" << std::endl;
                    renderAPI::set_uniform_value(program, data, name);
                }
                else if constexpr (std::is_same_v<T, Texture2D>){
                    data.bind();
                }
                else
                {
                    //std::cout << "Processing unknown type" << std::endl;
                    exit(1);
                }
            }
            void set_with_types(std::tuple<Args...>& data);
            std::tuple<Args...> m_values;
            std::vector<std::string> m_var_name;
    };

    //example
    /*static std::vector<std::string> names = {"ViewProjection", "spriteColor"};
    static IShaderLayout* layout = new ShaderLayout<glm::mat4, glm::vec3>({names.begin(), names.end()});*/
    struct  Uniform{
        std::string name;
        void *data;
        Uniform* pnext;
    };
    class Shader : public Layer{
        public:
            struct CreateInfo{
                const char* path;
                //IShaderLayout* layout;
            };
            //Shader(){};
            virtual ~Shader(){};
            static Shader* create_shader(CreateInfo info);
            virtual void use(Uniform* uniforms) = 0;
            virtual const Layout& get_attribute_layout() = 0;
            virtual const Layout& get_uniform_layout() = 0;
            /*virtual void set_uniform_value(glm::mat4, const char* name) = 0;
            virtual void set_uniform(glm::vec3 val, const char *name) = 0;
            virtual void set_uniform1i(int texture_index, const char* name) = 0;*/
//            virtual void on_update(engine_time::Time)override;
            //virtual IShaderLayout& get_shader_layout() = 0;
    };
}