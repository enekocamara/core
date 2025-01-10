#pragma once

#include <string>
#include <vector>
#include <expected>
#include <exception>

#include "Syris/utils/filesystem/readfile.h"
#include "Syris/include/OpenGL.h"
namespace Syris{
    inline std::expected<void, std::string> check_compile_errors(unsigned int shader_id)
    {
        GLint result = GL_FALSE;
        int info_log_length;

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0){
                CORE_ERROR(std::format("mem size {}", info_log_length));
                std::vector<char> vertex_shader_error_message(info_log_length + 1);
                glGetShaderInfoLog(shader_id, info_log_length, NULL, vertex_shader_error_message.data());
                vertex_shader_error_message[info_log_length] = '\0';
                return std::unexpected(std::format("opengl shader compilation error: [{}];", vertex_shader_error_message.data()));
            }else
                return std::unexpected("opengl shader compilation error: unknown;");
        }
        return std::expected<void, std::string>{};
    }
    inline std::expected<void, std::string> compile_shaders_source(int program_id, std::string &fragment_source, std::string &vertex_source)
    {
        unsigned int vertex_shader_id, fragment_shader_id;

        const char *vertex_source_c = vertex_source.c_str();
        const char *fragment_source_c = fragment_source.c_str();
        vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader_id, 1, &vertex_source_c, NULL);
        glCompileShader(vertex_shader_id);
        auto vertex_compile_result = check_compile_errors(vertex_shader_id);
        if (!vertex_compile_result)
            return std::unexpected(std::format("Failed to compile vertex shader: {}", vertex_compile_result.error()));
        // fragment Shader
        fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader_id, 1, &fragment_source_c, NULL);
        glCompileShader(fragment_shader_id);
        auto fragment_compile_result = check_compile_errors(fragment_shader_id);
        if (!fragment_compile_result)
            return std::unexpected(std::format("Failed to compile fragment shader: {}", fragment_compile_result.error()));

        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);
        glLinkProgram(program_id);

        // Check the program
        GLint result = GL_FALSE;
        int info_log_length;
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
            if (info_log_length > 0){
                CORE_ERROR(std::format("mem size {}", info_log_length));
                std::vector<char> ProgramErrorMessage(info_log_length + 1);
                glGetProgramInfoLog(program_id, info_log_length, NULL, &ProgramErrorMessage[0]);
                ProgramErrorMessage[info_log_length] = '\0';
                return std::unexpected(std::format("failed to link shader program: openGL error: [{}]", std::string(ProgramErrorMessage.data())));
            }else
                return std::unexpected("failed to link shader program: openGL error: unknown");
        }
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
        return std::expected<void, std::string>{};
    }
    
    inline std::expected<void, std::string> compile_shaders_2(int program_id, const char *vertex_shader_path, const char *fragment_shader_path)
    {
        //unsigned int vertex_shader_id, fragment_shader_id;
        std::string vertex_source = utils::read_file(vertex_shader_path).value();
        std::string fragment_source = utils::read_file(fragment_shader_path).value();
        auto res = compile_shaders_source(program_id, vertex_source, fragment_source);
        if (!res)
            return std::unexpected(std::format("failed to compile shaders from path: {}", res.error()));
        return std::expected<void, std::string>{};
    }

}