#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Syris/log/Log.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"
#include "Syris/renderer/Texture.h"

namespace Syris::renderAPI {
    /*
static void set_uniform_value(int program, glm::mat4 value, const char *name) {
  GLint location = glGetUniformLocation(program, name);
  if (location == -1) {
    CORE_ERROR(
        std::format("Uniform {} not found in shader program", name).c_str());
    exit(1);
  }
  glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}
static void set_uniform_value(int program, glm::vec3 value, const char *name) {
  glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
}

static void opengl_draw_quad2D(int program,
                               OpenGLVertexBuffer *buffer,
                               glm::mat4 model, glm::mat4 projection_view,
                               texture::Texture2D texture,
                               texture::Rectangle2D src, glm::vec3 color) {

  if (!glIsTexture(texture.m_id)) {
    CORE_ERROR(
        std::format("Invalid texture ID!: {}", texture.m_id).c_str());
    exit(1);
  }
  // Update texture coordinates in the VBO
  float updated_vertices[] = {
      // pos     //uv texture
      -1.0f, 1.0f,  src.min.x, src.max.y, // top-left
      1.0f,  1.0f,  src.max.x, src.max.y, // top-right
      1.0f,  -1.0f, src.max.x, src.min.y, // bottom-right

      -1.0f, 1.0f,  src.min.x, src.max.y, // top-left
      1.0f,  -1.0f, src.max.x, src.min.y, // bottom-right
      -1.0f, -1.0f, src.min.x, src.min.y, // bottom-left
  };
 
  // Bind vao vbo
  // buffer->bind();
  CHECK_GL_ERROR();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updated_vertices),
                  updated_vertices);
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
} // namespace Syris::renderAPI
*/