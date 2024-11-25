#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/texture/Texture.hpp"
#include "Syris/log/Log.h"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"

namespace Syris::renderAPI{
    static void set_uniform_value(int program, glm::mat4 value, const char *name)
    {
        GLint location = glGetUniformLocation(program, name);
        if (location == -1)
        {
            CORE_ERROR(std::format("Uniform {} not found in shader program", name));
            exit(1);
        }
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
    }
    static void set_uniform_value(int program, glm::vec2 value, const char *name)
    {
        GLint location = glGetUniformLocation(program, name);
        if (location == -1)
        {
            CORE_ERROR(std::format("Uniform {} not found in shader program", name));
            exit(1);
        }
        glUniform2f(glGetUniformLocation(program, name), value.x, value.y);
    }
    static void set_uniform_value(int program, glm::vec3 value, const char *name)
    {
        GLint location = glGetUniformLocation(program, name);
        if (location == -1)
        {
            CORE_ERROR(std::format("Uniform {} not found in shader program", name));
            exit(1);
        }
        glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
    }
    static void set_uniform_value(int program, Texture2D* texture, const char *name){
        //glActiveTexture(GL_TEXTURE0);
        texture->bind();
        //glUniform1i(glGetUniformLocation(program, name), 0);
    }
    static void set_uniform_value(int program, glm::vec4 value, const char *name)
    {
        GLint location = glGetUniformLocation(program, name);
        if (location == -1)
        {
            CORE_ERROR(std::format("Uniform {} not found in shader program", name));
            exit(1);
        }
        glUniform4f(glGetUniformLocation(program, name), value.x, value.y, value.z, value.w);
    }

    static void opengl_draw_quad2D(int program, OpenGLVertexBuffer *buffer, glm::mat4 model, glm::mat4 projection_view, Texture2D& texture, Rectangle2D src, glm::vec3 color)
    {
        if (!glIsTexture(texture.get_id()))
        {
            CORE_ERROR(std::format("Invalid texture ID!: {}", texture.get_id()));
            exit(1);
        }
        // Update texture coordinates in the VBO
        float updated_vertices[] = {
            // pos     //uv texture
            -1.0f, 1.0f, src.min.x, src.max.y, // top-left
            1.0f, 1.0f, src.max.x, src.max.y,  // top-right
            1.0f, -1.0f, src.max.x, src.min.y, // bottom-right

            -1.0f, 1.0f, src.min.x, src.max.y,  // top-left
            1.0f, -1.0f, src.max.x, src.min.y,  // bottom-right
            -1.0f, -1.0f, src.min.x, src.min.y, // bottom-left
        };
        /*float updated_vertices[] = {
            // positions   // texture coords
            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f,  1.0f,  1.0f, 1.0f,  // top-right
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right

            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f,  0.0f, 0.0f   // bottom-left
        };*/

        // Bind vao vbo
        // buffer->bind();
        CHECK_GL_ERROR();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updated_vertices), updated_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        set_uniform_value(program, projection_view, "ViewProjection");
        set_uniform_value(program, model, "model");
        // set_uniform_value(program, model, "model");

        set_uniform_value(program, color, "spriteColor");

        glActiveTexture(GL_TEXTURE0);
        texture.bind();
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Unuse the shader program
    }
    static void set_uniform_value(int program, const char *name, Type type, void *value){
        switch (type){
            case Type::vec2:
                set_uniform_value(program, *(glm::vec2*)value, name);
                break;
            case Type::vec3:
                set_uniform_value(program, *(glm::vec3*)value, name);
                break;
            case Type::vec4:
                set_uniform_value(program, *(glm::vec4*)value, name);
                break;
            case Type::mat4:
                set_uniform_value(program, *(glm::mat4*)value, name);
                break;
            case Type::texture2D:
                set_uniform_value(program, (Texture2D*)value, name);
                break;
            default:
                throw std::runtime_error("type not yet implemented in set uniform value opengl");
        }
    }
}
