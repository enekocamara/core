#include <vector>
#include <tuple>

#include "glad/glad.h"
#include "OpenGLShader.hpp"
#include "Syris/utils/file/readfile.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/include/imgui.h"
namespace Syris{
    void checkCompileErrors(unsigned int shader_id){
        GLint result = GL_FALSE;
        int info_log_length;

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            std::vector<char> vertex_shader_error_message(info_log_length + 1);
            glGetShaderInfoLog(shader_id,info_log_length, NULL, &vertex_shader_error_message[0]);
            printf("%s\n", &vertex_shader_error_message[0]);
            throw std::runtime_error(&vertex_shader_error_message[0]);
        }


    }
    void compile_shaders(int program_id,const char *vertex_shader_path, const char *fragment_shader_path){ 
        unsigned int vertex_shader_id, fragment_shader_id;
        std::string vertex_source = utils::readFile(vertex_shader_path);
        std::string fragment_source = utils::readFile(fragment_shader_path);

        const char * vertex_source_c = vertex_source.c_str();
        const char * fragment_source_c = fragment_source.c_str();

        vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader_id, 1, &vertex_source_c, NULL);
        glCompileShader(vertex_shader_id);
        checkCompileErrors(vertex_shader_id);
        // fragment Shader
        fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader_id, 1, &fragment_source_c, NULL);
        glCompileShader(fragment_shader_id);
        checkCompileErrors(fragment_shader_id);

        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);
        glLinkProgram(program_id);

        // Check the program
        GLint result = GL_FALSE;
        int info_log_length;
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            std::vector<char> ProgramErrorMessage(info_log_length + 1);
            glGetProgramInfoLog(program_id, info_log_length, NULL, &ProgramErrorMessage[0]);
            throw std::runtime_error(&ProgramErrorMessage[0]);
        }
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

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

    void OpenGLShader::use(void *uniforms /*TODO!!!*/) {
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
    void OpenGLShader::on_update(engine_time::Time& time){
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
        /*ImGui::Begin("Shader Settings");
        if (ImGui::Button("Recompile shader"))
        {
            
            // This code will run when the button is clicked
            //std::cout << "Button clicked!" << std::endl;
        }
        ImGui::End();*/
    }
}
