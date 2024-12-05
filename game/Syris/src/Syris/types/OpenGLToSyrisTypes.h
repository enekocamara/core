#pragma once
#include <expected>
#include <string>
#include "Type.h"
#include "Syris/include/OpenGL.h"
#include "Syris/utils/containers/BiMap.hpp"
#include "Syris/utils/Breakpoint.h"

namespace Syris{

    static const std::vector<const char *> openGL_type_to_str{
        "f32",
        "i32",
        "u32",
        "vec2",
        "uvec2",
        "ivec2",
        "vec3",
        "uvec3",
        "ivec3",
        "vec4",
        "uvec4",
        "ivec4",
        "mat4",
        "texture",
    };
    class OpenGLTypes{
        public:
        OpenGLTypes(){
            m_types.add(GL_FLOAT, Type::f32);
            m_types.add(GL_INT, Type::i32);
            m_types.add(GL_UNSIGNED_INT, Type::u32);
            m_types.add(GL_FLOAT_VEC2, Type::vec2);
            m_types.add(GL_INT_VEC2, Type::ivec2);
            m_types.add(GL_UNSIGNED_INT_VEC2, Type::uvec2);
            m_types.add(GL_FLOAT_VEC3, Type::vec3);
            m_types.add(GL_INT_VEC3, Type::ivec3);
            m_types.add(GL_UNSIGNED_INT_VEC3, Type::uvec3);
            m_types.add(GL_FLOAT_VEC4, Type::vec4);
            m_types.add(GL_INT_VEC4, Type::ivec4);
            m_types.add(GL_UNSIGNED_INT_VEC4, Type::uvec4);
        }
        static const BiMap<int, Type>& types(){
            static OpenGLTypes types = OpenGLTypes();
            return types.get_types();
        };
        const BiMap<int, Type>& get_types() const{return m_types;}
        private:
            BiMap<int, Type> m_types;
    };

    class GLSLTypes{
    public:
        GLSLTypes()
        {
            m_types.add(Type::f32, "float");
            m_types.add(Type::i32, "int");
            m_types.add(Type::u32, "uint");
            m_types.add(Type::vec2, "vec2");
            m_types.add(Type::uvec2, "uvec2");
            m_types.add(Type::ivec2, "ivec2");
            m_types.add(Type::vec3, "vec3");
            m_types.add(Type::uvec3, "uvec3");
            m_types.add(Type::ivec3, "ivec3");
            m_types.add(Type::vec4, "vec4");
            m_types.add(Type::uvec4, "uvec4");
            m_types.add(Type::ivec4, "ivec4");
            m_types.add(Type::mat4, "mat4");
            m_types.add(Type::texture2D, "sampler2D"); // Assuming you're referring to a 2D texture
        }
        static const BiMap<Type, std::string> &types()
        {
            static GLSLTypes types = GLSLTypes();
            return types.get_types();
        };
        static const std::string& convert(Type type){
            auto it = types().from<Type>().find(type);
            if (it == types().from<Type>().end())
                BREAK_POINT("failed to convert type");
            return it->second;
        }
        const BiMap<Type, std::string> &get_types() const { return m_types; }

    private:
        BiMap<Type,std::string> m_types;
    };
    //static BiMap<int, const char *> openGL_types;
    static [[nodiscard]] const char * openGL_to_str(int openGL_type){
        return type::to_str(OpenGLTypes::types().from<int>().at(openGL_type));
    }

    static [[nodiscard]] std::expected<Type, std::string> openGL_to_syris_type(int openGL_type){
        switch (openGL_type){
            case GL_FLOAT:
                return Type::f32;
            case GL_INT:
                return Type::i32;
            case GL_UNSIGNED_INT:
                return Type::u32;
            case GL_FLOAT_VEC2:
                return Type::vec2;
            case GL_INT_VEC2:
                return Type::ivec2;
            case GL_UNSIGNED_INT_VEC2:
                return Type::uvec2;
            case GL_FLOAT_VEC3:
                return Type::vec3;
            case GL_INT_VEC3:
                return Type::ivec3;
            case GL_UNSIGNED_INT_VEC3:
                return Type::uvec3;
            case GL_FLOAT_VEC4:
                return Type::vec4;
            case GL_INT_VEC4:
                return Type::ivec4;
            case GL_UNSIGNED_INT_VEC4:
                return Type::uvec4;
            default:
               return std::unexpected("Type not recognised");
        }
    }
    static [[nodiscard]] std::expected<Type, std::string> glsl_to_syris_type(const std::string& type){
        const std::unordered_map<std::string, Type>& map = GLSLTypes::types().from<std::string>();
        const auto it = map.find(type);
        if (it == map.end())
            return std::unexpected("type not found");
        return it->second;
    }
    static [[nodiscard]] std::expected<Type, std::string> glsl_to_syris_type(std::string_view type) {
        const std::unordered_map<std::string, Type>& map = GLSLTypes::types().from<std::string>();
        const auto it = map.find(std::string(type));
        if (it == map.end())
            return std::unexpected("type not found");
        return it->second;
    }

    inline [[nodiscard]] std::string type_to_shader_script_type(Type type) {
        return GLSLTypes::convert(type);
    }
}