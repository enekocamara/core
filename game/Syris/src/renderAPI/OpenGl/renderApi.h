#pragma once
#include "renderer/Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace renderAPI{
    static void set_uniform_value(int program, glm::mat4 value, const char* name){
        glUniformMatrix4fv(glGetUniformLocation(program, name), 1, false, glm::value_ptr(value));
    }
    static void set_uniform_value(int program, glm::vec3 value, const char* name){
        glUniform3f(glGetUniformLocation(program, name),value.x, value.y, value.z);
    }

    static void draw_quad2D(int program, GLuint quad_vao, GLuint quad_vbo, glm::mat4 model, glm::mat4 projection_view, ecs::Texture2D texture, ecs::textures::Rectangle src, glm::vec3 color){
        if (!glIsTexture(texture.m_id)) {
            std::cerr << "Invalid texture ID! " << texture.m_id << std::endl;
            exit(1);
        }

        float tex_x_min = src.orig.x / texture.m_width;
        float tex_y_min = src.orig.y / texture.m_height;
        float tex_x_max = (src.orig.x + src.size.x) / texture.m_width;
        float tex_y_max = (src.orig.y + src.size.y) / texture.m_height;
        // Update texture coordinates in the VBO
        float updated_vertices[] = { 
            // pos     //uv texture 
            -1.0f,  1.0f,  tex_x_min, tex_y_max,  // top-left
            1.0f,  1.0f,  tex_x_max, tex_y_max,  // top-right
            1.0f, -1.0f,  tex_x_max, tex_y_min,  // bottom-right

            -1.0f,  1.0f,  tex_x_min, tex_y_max,  // top-left
            1.0f, -1.0f,  tex_x_max, tex_y_min,  // bottom-right
            -1.0f, -1.0f,  tex_x_min, tex_y_min   // bottom-left
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

        // Bind texture
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updated_vertices), updated_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        set_uniform_value(program, projection_view, "ViewProjection");
        set_uniform_value(program, model, "model");
        //set_uniform_value(program, model, "model");

        set_uniform_value(program, color, "spriteColor");

        glActiveTexture(GL_TEXTURE0);
        texture.bind();
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);

        // Bind VAO and draw
        glBindVertexArray(quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Unuse the shader program
    }
}
