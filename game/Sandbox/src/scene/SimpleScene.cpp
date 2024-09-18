#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SimpleScene.hpp"

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/shader/Shader.hpp"
#include "../world_generator/world_generator.hpp"
#include "../ecs/Components.h"
#include "../ecs/Tile.h"
namespace Sandbox{

    struct TileVertices{
        std::array<float, 6> vertices;
        TileVertices(int a){
            std::cout << "tile  vertices\n";
            this->vertices = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.0f, 0.5f
            };
        }
        /*std::pair<glm::vec2, glm::vec2> get_min_max() {
            return {{-1,-1},{-1,-1}}; 
        }*/
    };

    struct TileIndices{
        std::array<uint32_t, 6> vertices;
        TileIndices(int a){
            std::cout << "default indices\n";
            this->vertices = {
                0,1,2,2,3,0
            };
        }

    };
    struct TileInstancedData{
        glm::mat4 data;
        TileInstancedData(int a) {
            std::cout << "default instance data\n";
            data = glm::mat4(1.f);
        }
    };


    Syris::renderer::RenderBuffer* makeSimpleRenderBuffer(){
           
            TileIndices indices{ 0 };
            TileInstancedData instanced_data{0};
            TileVertices vertices{0};

            
            using AttCreateInfo = Syris::renderer::AttributeLayout::CreateInfo;
            //define per vertex layout
            AttCreateInfo layout_pos{
                .values_count = 2,
                .value_type= Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = false,
            };
            //per vertex layout list
            std::array<AttCreateInfo, 1> vertex_layouts_info =  {layout_pos}; 
            Syris::renderer::AttributeLayoutList vertex_attribute_list({vertex_layouts_info.begin(), vertex_layouts_info.end()}, 0); 

            //define per instance layout
            AttCreateInfo instanced_model_col_0{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_1{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_2{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            AttCreateInfo instanced_model_col_3{
                .values_count = 4,
                .value_type = Syris::renderer::ValueType::Float,
                .normalize = false,
                .perInstance = true
            };
            //per instance layout list
            
            
            std::array<AttCreateInfo, 4> instanced_layouts_info =  {instanced_model_col_0, instanced_model_col_1, instanced_model_col_2, instanced_model_col_3}; 
            Syris::renderer::AttributeLayoutList instance_attribute_list({instanced_layouts_info.begin(), instanced_layouts_info.end()}, vertex_attribute_list.attribute_size()); 

            using BufferInfo = Syris::renderer::VertexBuffer::BufferInfo;
            //define vertex and instance buffer info for render buffer:
            BufferInfo vertex_buffer{
                .layout_list = vertex_attribute_list,
                .size = sizeof(TileVertices),
                .data = vertices.vertices.data()
            };
            BufferInfo instance_buffer{
                .layout_list = instance_attribute_list,
                .size = sizeof(TileInstancedData),
                .data = &instanced_data
            };

            Syris::renderer::RenderBuffer::CreateInfo render_buffer_info{
                .dynamic = true,
                .index_count = 6,
                .indices = reinterpret_cast<uint32_t *>(&indices),
                .per_vertex_buffer_info = vertex_buffer,
                .per_instance_buffer_info = instance_buffer,
            };
            Syris::renderer::RenderBuffer *buffer = new Syris::renderer::RenderBuffer(render_buffer_info);
            return buffer;
    }
    SimpleScene::SimpleScene(CreateInfo info): m_graphics_context(info.context), m_camera(info.camera_info){
        const char * vertex_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\simple_vertex_shader.glsl";
        const char * fragment_shader_path = "C:\\Users\\eneko\\dev\\asharis\\game\\Sandbox\\shaders\\simple_fragment_shader.glsl";
        m_shader_id = m_graphics_context.get_shader_manager().add_shader({fragment_shader_path, vertex_shader_path});
        if (!m_shader_id){
            throw std::runtime_error("failed to add shader");
        }
        auto texture = ecs::Tile::defaultTextureBundle();
        texture.src = texture::atlas::grass_0;
        //ecs::Tile::newTile(glm::vec2(0, 0), texture, m_registry, ecs::CTile::TileType::Grass);
        m_buffer = makeSimpleRenderBuffer();
        Syris::Logger::client_info("simple scene successfully created");
        CHECK_GL_ERROR();
    }
    void SimpleScene::on_update(Syris::engine_time::Time& time){
        //it now owns the camera
        /*outdated by renderbuffer??*/
        /*
        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture,
                ecs::CPosition, ecs::CTile>);
        uint32_t index = 0;
        for (auto entity : render_group) {
            auto [cTexture, cPosition] =
                render_group.get<ecs::CTexture, ecs::CPosition>(entity);
            Syris::texture::Texture2D texture = m_texture_atlas.getTexture();
            //std::cout << "min: " << cTexture.rect.min << " max: " << cTexture.rect.max << '\n';
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                    model, glm::vec3(cPosition.pos,
                        0.0f)); // first translate (transformations are: scale
                                // happens first, then rotation, and then final
                                // translation happens; reversed order)

            
            //   model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y,
             //  0.0f)); // move origin of rotation to center of quad model =
             //  glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); //
             //  then rotate model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f
            //  size.y, 0.0f)); // move origin back
            
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f)); // last scale
            
            CHECK_GL_ERROR();
            m_buffer->bind(0);
            float updated_vertices[] = { 
                // pos     //uv texture 
                -1.0f, 1.0f,  cTexture.rect.min.x, cTexture.rect.max.y, // top-left
                1.0f,  1.0f,  cTexture.rect.max.x, cTexture.rect.max.y, // top-right
                1.0f,  -1.0f, cTexture.rect.max.x, cTexture.rect.min.y, // bottom-right
                -1.0f, -1.0f, cTexture.rect.min.x, cTexture.rect.min.y  // bottom-left
            };
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileVertices), sizeof(TileVertices), updated_vertices );
            CHECK_GL_ERROR();
            m_buffer->bind(1);
            glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(TileInstancedData), sizeof(glm::mat4), &model );
            index++;
            CHECK_GL_ERROR();
            
        }
        m_buffer->bind(0);
        //glUseProgram(program);already done by graphics context on update
        //float buffer[1000];
        //glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
        //TileVertices * vertices = reinterpret_cast<TileVertices*>(buffer);
        //int i = 0;
        //std::cout << "after\n\n\n";
        //while (i < 1000 / 16){
        //    std::cout << "min : " << vertices[1].get_min_max().first << "max : " << vertices[i].get_min_max().second << '\n';
        //    i++;
        //};
        */

        //OpenGLContext* context_opengl = reinterpret_cast<OpenGLContext*>(context);
        Syris::Shader* shader = m_graphics_context.get_shader_manager().get_shader(m_shader_id);
        shader->use();
        //m_graphics_context.get_shader_manager().use_shader(m_shader_id); //shader->use();
        m_buffer->bind(0); //m_vertexBuffer->bind(1); m_indexBuffer->bind();
        
        shader->set_uniform_value(m_camera.getCamera().get_view_projection_matrix(), "ViewProjection");
//        shader->set_uniform_value(glm::mat4(1.f), "ViewProjection");
        
        //renderAPI::set_uniform_value(context_opengl->get_program(), projection_view, "ViewProjection");
        //glActiveTexture(GL_TEXTURE0);!!TODO

        //shader->set_uniform(glm::vec3(1.f), "spriteColor");
        //renderAPI::set_uniform_value(context_opengl->get_program(), glm::vec3(1.f), "spriteColor");
        CHECK_GL_ERROR();
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
        CHECK_GL_ERROR();
        //Syris::Logger::client_info("drawing...\n");
        //m_buffer->draw_buffer(m_graphics_context.get_shader_manager().get_shader(m_shader_id), m_camera.getCamera().get_view_projection_matrix(), m_texture_atlas.getTexture(), 100 * 100);
    }
    bool SimpleScene::on_event(Syris::Event* event){
        return false;
    }
    SimpleScene::~SimpleScene(){

    }
}
