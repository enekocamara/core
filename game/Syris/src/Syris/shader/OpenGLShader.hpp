#pragma once
#include "Syris/Libs.h"
#include "Shader.hpp"
#include <glm/glm.hpp>

namespace Syris{
    void compile_shaders(int program_id,const char *vertex_shader_path, const char *fragment_shader_path);
    class OpenGLShader : public Shader{
    public:
        OpenGLShader(Shader::CreateInfo info);
        ~OpenGLShader();
        void use();
        void set_uniform_value(glm::mat4, const char *name)override;
        void set_uniform(glm::vec3 val, const char *name)override;
        void set_uniform1i(int texture_index, const char *name)override;

    private:
        int m_program;
    };
}
