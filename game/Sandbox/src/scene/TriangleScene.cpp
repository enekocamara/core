#include "TriangleScene.hpp"
#include "Syris/shader/Shader.hpp"
namespace Sandbox{

    TriangleScene::TriangleScene(CreateInfo info):
        m_graphics_context(info.context),
        m_camera(info.camera_info){

        //auto shader_layout = ShaderLayout<>;
        const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\triangle_vertex_shader.glsl";
        const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\triangle_fragment_shader.glsl";
        m_shader_id = m_graphics_context.get_shader_manager().add_shader({fragment_shader_path, vertex_shader_path});
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
    }
    TriangleScene::~TriangleScene(){

    }

    void TriangleScene::on_update(){
        m_graphics_context.get_shader_manager().use_shader(m_shader_id);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3); 
        glBindVertexArray(m_vao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    bool TriangleScene::on_event(Syris::Event *event) {
        return false;
    }
}