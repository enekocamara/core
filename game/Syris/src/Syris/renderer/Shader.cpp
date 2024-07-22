#include "Shader.hpp"
#include "Syris/utils/file/readfile.h"
namespace shader{
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
    void compile_shader(int program_id,const char *vertex_shader_path, const char *fragment_shader_path){ 
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

        glDetachShader(program_id, vertex_shader_id);
        glDetachShader(program_id, fragment_shader_id);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

}
