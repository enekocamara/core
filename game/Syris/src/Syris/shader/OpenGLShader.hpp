#pragma once
#include "Syris/Libs.h"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include "Syris/utils/EngineTime.h"
#include "Syris/events/Event.hpp"
#include <filesystem>

namespace Syris{
    void compile_shaders(int program_id,const char *vertex_shader_path, const char *fragment_shader_path);
    class OpenGLShader : public Shader{
    public:
        OpenGLShader(Shader::CreateInfo info);
        ~OpenGLShader(){};
        void use(void *uniforms);
        void set_uniform_value(glm::mat4, const char *name)override;
        void set_uniform(glm::vec3 val, const char *name)override;
        void set_uniform1i(int texture_index, const char *name)override;
        void on_update(engine_time::Time& time)override;
        bool on_event(Event* event)override{return false;}; 
    private:
        int m_program;
        CreateInfo m_info;
        std::filesystem::file_time_type m_last_change;
        std::string m_vertex_path;
        std::string m_fragment_path;
        IShaderLayout* m_layout;
    };
}
