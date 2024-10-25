#pragma once
#include "TriangleScene.hpp"
#include "Syris/shader/Shader.hpp"
#include "Syris/renderAPI/OpenGl/OpenGLrenderApi.h"
#include "Syris/statistics/Components.hpp"
#include "../shaders/triangle_scene_layout.h"
namespace Sandbox{

    TriangleScene::TriangleScene(CreateInfo info):
        m_graphics_context(info.context),
        m_camera(info.camera_info){

        //auto shader_layout = ShaderLayout<>;
        //const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\triangle_vertex_shader.glsl";
        //const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\triangle_fragment_shader.glsl";
        Syris::Statistics::AddModuleInfo mod_info{};
        auto id = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<Syris::statistics::CScene>(id, "Triangle Scene");

        Syris::Logger::client_info("triangle scene being created");
        m_shader_id = m_graphics_context.get_shader_manager().add_shader(triangle_scene::get_shader_info());
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f};

        glGenBuffers(1, &m_vbo);
        glGenVertexArrays(1, &m_vao); 

        glBindVertexArray(m_vao);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        m_triangle_color = glm::vec3(1.0,0.0,0.0);
        Syris::Logger::client_info("triangle scene successfully created");
    }
    TriangleScene::~TriangleScene(){

    }

    void TriangleScene::on_update(Syris::engine_time::Time& time){
        m_graphics_context.get_shader_manager().get_shader(m_shader_id)->on_update(time);
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("color 1", reinterpret_cast<float *>(&m_triangle_color)); 

        ImGui::End();
        triangle_scene::ShaderLayoutTuple data = m_triangle_color;
        m_graphics_context.get_shader_manager().use_shader(m_shader_id, &data);
        //Syris::Shader* shader = m_graphics_context.get_shader_manager().get_shader(m_shader_id);
        //shader->set_uniform(m_triangle_color, "uColor");
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        glBindVertexArray(m_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    bool TriangleScene::on_event(Syris::Event *event) {
        return false;
    }
}