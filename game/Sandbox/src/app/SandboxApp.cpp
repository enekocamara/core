#include "SandboxApp.hpp"
#include "../scene/SandboxScene.hpp"
#include "ecs/Components.h"
#include "renderer/Texture.h"
#include "renderer/Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SandboxApp::SandboxApp(SandboxApp::CreateInfo info) : m_window(info.window_bundle),
                                                       m_scene(new SandboxScene(info.registry)),
                                                       m_renderer(info.registry),
                                                       m_registry(info.registry),
                                                       m_camera(info.cameraCreateInfo)
{
   m_program = glCreateProgram();
   shader::compile_shader(m_program, info.vertex_shader_path, info.fragment_shader_path); 
   m_texture_atlas.init();
}
SandboxApp::~SandboxApp(){

}

void SandboxApp::run(){
    while(!m_window.shouldWindowClose()){
        m_window.onUpdateStart();
        draw_frame();
        m_window.onUpdateEnd();
    }
}

void SandboxApp::draw_frame(){
    glUseProgram(m_program);
    /*glBindVertexArray(quad.m_vertex_array);

    glActiveTexture(GL_TEXTURE0);
    m_texture_atlas.getTexture(ecs::TextureKeys::Bush).bind();
    glUniform1i(glGetUniformLocation(m_program, "texture1"), 0);

    glDrawArrays(GL_TRIANGLES, 0,6);
    */
    
    auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition, ecs::CTile>) ;
    draw_group(render_group);
    
}

template<typename Group>
void SandboxApp::draw_group(Group& group){
    float ratio = 1920.f / 1080.f;
    for (auto entity : group){
        auto[cTexture, cPosition] = group.get<ecs::CTexture, ecs::CPosition>(entity);
        ecs::Texture2D texture = m_texture_atlas.getTexture(cTexture.texture.src.key);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cPosition.pos, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        /*
        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
*/
        model = glm::scale(model, glm::vec3(0.5f,0.5f,1.0f)); // last scale

        m_renderer.render_quad(m_program, quad, model, m_camera.get_view_projection_matrix(),texture, cTexture.texture.src.rect ,glm::vec3(1.f));
        //std::cout << "position: " << cPosition.pos << '\n';
    }
    //exit(1);
}
