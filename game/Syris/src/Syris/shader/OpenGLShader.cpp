/*
#include <vector>
#include <tuple>

#include "glad/glad.h"
#include "OpenGLShader.hpp"
#include "Syris/utils/file/readfile.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/include/imgui.h"
#include "OpenGLUtils.h"
namespace Syris{
    OpenGLShader::OpenGLShader(CreateInfo info){
        m_program = glCreateProgram();
        m_info = info;
        m_vertex_path = std::format("{}\\vertex.glsl", info.path);
        m_fragment_path = std::format("{}\\fragment.glsl", info.path);
        compile_shaders(m_program, m_vertex_path.c_str(), m_fragment_path.c_str());
        info.layout->check_layout(m_program);
        m_last_change = std::max(std::filesystem::last_write_time(m_fragment_path), std::filesystem::last_write_time(m_vertex_path));
        m_layout = info.layout;
        m_layout->get();
    }

    void OpenGLShader::use(void *uniforms)//todo {
        glUseProgram(m_program);
        auto hold = std::make_tuple(m_program, uniforms);
        m_layout->set(&hold);
        CHECK_GL_ERROR();
    }
    void OpenGLShader::set_uniform_value(glm::mat4 val, const char *name){ 
        renderAPI::set_uniform_value(m_program, val, name);
    }
    void OpenGLShader::set_uniform(glm::vec3 val, const char *name) {
        renderAPI::set_uniform_value(m_program, val, name);
    }
    void OpenGLShader::set_uniform1i(int texture_index, const char *name) {        
        glUniform1i(glGetUniformLocation(m_program, name), texture_index);
    }
    void OpenGLShader::on_update(const engine_time::Time & time) {
        if (m_last_change != std::max(std::filesystem::last_write_time(m_vertex_path), std::filesystem::last_write_time(m_fragment_path))){
            int hold = glCreateProgram();
            // m_program = glCreateProgram();
            try
            {
                compile_shaders(hold, m_vertex_path.c_str(), m_fragment_path.c_str());
                m_program = hold;
                m_last_change = std::max(std::filesystem::last_write_time(m_vertex_path), std::filesystem::last_write_time(m_fragment_path));
            }
            catch (std::runtime_error &error)
            {
                std::cerr << "failed to compile!" << std::endl;
                CORE_ERROR("failed to compile shader");
            }
        }
    }
}
*/