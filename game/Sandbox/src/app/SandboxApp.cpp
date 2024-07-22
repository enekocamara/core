#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SandboxApp.hpp"
#include "../scene/SandboxScene.hpp"
#include "../ecs/Components.h"
#include "Syris/renderer/Texture.h"
#include "Syris/renderer/Shader.hpp"
#include "Syris/input/Input.h"
namespace Sandbox{
    SandboxApp::SandboxApp(SandboxApp::CreateInfo info) : m_window(info.window_bundle),
    m_texture_atlas(info.atlas_path),
    m_scene(new SandboxScene(info.registry)),
    m_renderer(info.registry),
    m_registry(info.registry),
    m_camera(info.cameraCreateInfo)
    {
        m_program = glCreateProgram();
        shader::compile_shader(m_program, info.vertex_shader_path, info.fragment_shader_path); 
        m_texture_atlas.init();
        m_time.init_time = std::chrono::high_resolution_clock::now();
        Syris::Input::get(m_window.getWindow());

        /*temp quad creation*/
        float vertices[] = { 
            // pos     //uv texture 
            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f,  1.0f,  1.0f, 1.0f,  // top-right
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right

            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f,  0.0f, 0.0f   // bottom-left
        };

        std::vector<Syris::VertexBuffer::AttributeLayout> layouts(2);
        Syris::VertexBuffer::AttributeLayout layout_pos = {};
        layout_pos.index_layout = 0;
        layout_pos.values_count = 2;
        layout_pos.normalize = false;
        layout_pos.stride_count = 4;
        layout_pos.skip_count = 0;
        
        Syris::VertexBuffer::AttributeLayout layout_tex = {};
        layout_tex.index_layout = 1;
        layout_tex.values_count = 2;
        layout_tex.normalize = false;
        layout_tex.stride_count = 4;
        layout_tex.skip_count = 2;
       
        layouts[0] = layout_pos;
        layouts[1] = layout_tex;

        Syris::VertexBuffer::CreateInfo buffer_info = {};
        buffer_info.data = vertices;
        buffer_info.dynamic = true;
        buffer_info.vertices_count = 6;
        buffer_info.vertice_size = sizeof(float) * 4;
        buffer_info.layouts = layouts.data();
        buffer_info.layouts_count = layouts.size();

        quad = new Syris::renderAPI::Quad2D(buffer_info);
    }
    
    SandboxApp::~SandboxApp(){
        delete quad;
    }

    void SandboxApp::run(){
        m_time.last_frame = std::chrono::high_resolution_clock::now();
        while(!m_window.shouldWindowClose()){
            CHECK_GL_ERROR();
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();    
            m_time.delta_time_ms = std::chrono::duration<float, std::milli>(now - m_time.last_frame).count();
            glUseProgram(m_program);
            m_window.onUpdateStart();
            m_camera.on_update(m_time);
            draw_frame();
            m_window.onUpdateEnd();
            CHECK_GL_ERROR();
        }
    }

    void SandboxApp::draw_frame(){
            CHECK_GL_ERROR();

        glUseProgram(m_program);
        /*glBindVertexArray(quad.m_vertex_array);

          glActiveTexture(GL_TEXTURE0);
          m_texture_atlas.getTexture(ecs::TextureKeys::Bush).bind();
          glUniform1i(glGetUniformLocation(m_program, "texture1"), 0);

          glDrawArrays(GL_TRIANGLES, 0,6);
          */

        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition, ecs::CTile>) ;
        quad->buffer->unbind();
        draw_group(render_group);
        CHECK_GL_ERROR();

    }

    template<typename Group>
        void SandboxApp::draw_group(Group& group){
            float ratio = 1920.f / 1080.f;
            for (auto entity : group){
                auto[cTexture, cPosition] = group.get<ecs::CTexture, ecs::CPosition>(entity);
                Syris::texture::Texture2D texture = m_texture_atlas.getTexture();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(cPosition.pos, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

                /*
                   model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
                   model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
                   model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
                   */
                model = glm::scale(model, glm::vec3(0.5f,0.5f,1.0f)); // last scale

                CHECK_GL_ERROR();
                m_renderer.draw_quad(m_program, quad, model, m_camera.getCamera().get_view_projection_matrix(),texture, cTexture.rect ,glm::vec3(1.f));
                //std::cout << "position: " << cPosition.pos << '\n';
            }
            //exit(1);
        }
}
