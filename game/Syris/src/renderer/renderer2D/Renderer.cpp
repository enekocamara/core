#include "Renderer.h"
#include "../../ecs/Components.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderAPI/renderApi.h"
namespace ge {
    Renderer2D::Renderer2D(entt::registry& registry) : m_registry(registry){
        this->time.init_time = std::chrono::high_resolution_clock::now(),
        this->textureAtlas.init();

    }
    Renderer2D::~Renderer2D(){
        this->state = GameEngineState::WindowClosed;
    }
    /*
    void Renderer2D::logic(){
        this->handle_tick();
        this->handle_interactable();
        this->handleKeys();
        if (this->interactable != std::nullopt && IsKeyDown(KEY_Q)){
            this->m_registry.get<ecs::CInteractable>(this->interactable.value()).interact(this->m_registry, this->interactable.value());
        }
        auto key_group = this->m_registry.group<ecs::CKeyBinded>(entt::get<ecs::CDir, ecs::CSpeed>);
        for (auto entity : key_group){
            auto[cKeyBinded, cDir, cSpeed] = key_group.get<ecs::CKeyBinded, ecs::CDir, ecs::CSpeed>(entity); 
            auto previous = cDir.value;
            cSpeed.value = 0;
            cDir.value = {0.f,0.f};
            if (this->keys[cKeyBinded.keys.up]){
                cDir.value.y = -1;
                cSpeed.value = 1;
            }
            else if (keys[cKeyBinded.keys.down]){
                cDir.value.y = 1;
                cSpeed.value = 1;
            }
            else if (keys[cKeyBinded.keys.right]){
                cDir.value.x = 1;
                cSpeed.value = 1;
            }
            else if (keys[cKeyBinded.keys.left]){
                cDir.value.x = -1;
                cSpeed.value = 1;
            }

            if (cDir.value.x + cDir.value.y == 0)
                cDir.value = previous; 
            else
                cDir.value = glm::normalize(cDir.value);
            cSpeed.value *= (float)config::render_tile_size * 3;
        }
                
        auto movement_group = this->m_registry.group<ecs::CPosition>(entt::get<ecs::CSpeed, ecs::CDir>);
        for (auto entity : movement_group){
            auto[cPosition, cSpeed, cDir] = movement_group.get<ecs::CPosition, ecs::CSpeed, ecs::CDir>(entity); 
            cPosition.pos += cDir.value * (cSpeed.value * this->time.delta_time_ms / 1000.f);
        } 
        auto animated_group = this->m_registry.group<ecs::CAnimated>(entt::get<ecs::CTexture>);
        for (auto entity : animated_group){
            auto[cAnimated, cTexture] = animated_group.get<ecs::CAnimated, ecs::CTexture>(entity); 
            cAnimated.animate(this->m_registry, entity, this->time, cTexture.texture);
        } 
    }*/
    void Renderer2D::render_quad(int program, Quad2D quad, glm::mat4 model, glm::mat4 projection_view, ecs::Texture2D texture, ecs::textures::Rectangle src, glm::vec3 color){
        renderAPI::draw_quad2D(program, quad.m_vertex_array, quad.m_vertex_buffer, model, projection_view, texture, src, color); 
    }
    /*
    void Renderer2D::renderFrame(){
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

        model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

        this->shader.SetMatrix4("model", model);

        // render textured quad
        this->shader.SetVector3f("spriteColor", color);

        glActiveTexture(GL_TEXTURE0);
        texture.Bind();

        glBindVertexArray(this->quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);

        ecs::TextureBundle player_texture = this->m_registry.get<ecs::CTexture>(this->player).texture;
        glm::vec2 player_pos = this->m_registry.get<ecs::CPosition>(this->player).pos;

        this->camera.target = {player_pos.x + player_texture.size.x / 2.0f, player_pos.y + player_texture.size.y / 2.0f};
        BeginMode2D(this->camera);

        auto render_group = this->m_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition, ecs::CTile>);
        this->draw_group<true>(render_group);
        auto render_group2 = this->m_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition>, entt::exclude<ecs::CTile>);
        // render_group2.sort();
        render_group2.sort<ecs::CPosition, ecs::CTexture>([](const auto lhs, const auto rhs) {
                const auto& [lhs_pos, lhs_tex] = lhs;
                const auto& [rhs_pos, rhs_tex] = rhs;
                return lhs_pos.pos.y + lhs_tex.texture.size.y < rhs_pos.pos.y + rhs_tex.texture.size.y;
        });

        this->draw_group<false>(render_group2);
        EndMode2D();
        this->draw_interact();
        DrawText(std::format("POS [{}]", player_pos.y).c_str(), 100, 30, 20, RED);
        DrawFPS(0, 0);
        EndDrawing();
    }
    void Renderer2D::draw_interact(){
        if (this->interactable != std::nullopt){
            DrawText(std::format("{} (q)", this->m_registry.get<ecs::CInteractable>(this->interactable.value()).message).c_str(), this->screen_size.x / 2 - 50,this->screen_size.y / 5 * 4, 20, WHITE);
        }
    }
    template<bool debug, typename T>
    void Renderer2D::draw_group(T& render_group){
        constexpr bool render_debug = false;
        for (auto entity : render_group){
            auto[cTexture, cPosition] = render_group.get<ecs::CTexture, ecs::CPosition>(entity);
            Texture texture = this->textureAtlas.getTexture(cTexture.texture.src.key);
            if (texture.id == 0){
                std::cout << "texture uninitialized\n";
                exit(1);
            }
            DrawTexturePro(
                    texture,
                    cTexture.texture.src.rect,
                    Rectangle{
                    .x = cPosition.pos.x,
                    .y = cPosition.pos.y,
                    .width = cTexture.texture.size.x,
                    .height = cTexture.texture.size.y,
                    },{0,0},0,cTexture.texture.color
                    );
        }
        
    }
    void Renderer2D::handle_interactable(){
        auto interact_group = this->m_registry.group<>(entt::get<ecs::CInteractable, ecs::CPosition>);
        glm::vec2 player_pos;/// = this->m_registry.get<ecs::CPosition>(this->player).pos;

        float min_distace;
        entt::entity min_entity;
        bool first = true;
        for (auto entity : interact_group){
            glm::vec2 pos = interact_group.get<ecs::CPosition>(entity).pos;
            float distance = glm::distance(player_pos, pos);
            if ((first || distance < min_distace) && distance < 16){
                ecs::CInteractable interactable = interact_group.get<ecs::CInteractable>(entity);
                if (interactable.can_interact(this->m_registry, entity)){
                    min_distace = distance;
                    min_entity = entity;
                    first = false;
                }
            }
        }
        if (first){
            this->interactable = std::nullopt; 
            return;
        }
        if (min_distace < 16){
            this->interactable = min_entity;
        }
 
    }
    
    void Renderer2D::handle_tick(){
        auto tick_group = this->m_registry.group<>(entt::get<ecs::CTick>);

        for (auto entity : tick_group){
            tick_group.get<ecs::CTick>(entity).tick(this->m_registry, entity, this->time);
        }
    }
    void Renderer2D::handleKeys(){
        for (int i = 1; i < MAX_NUMBER_KEYS; i++){
            this->keys[i] = IsKeyDown(i);
        }    
    } 
*/

    Triangle2D::Triangle2D(){
        float vertices[] = { 
            // pos      // tex
            -0.5f, -.5f,
            0.5f, -0.5f,
            0.0f, 0.5f,
        };
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    }
    Triangle2D::~Triangle2D(){

    }
    Quad2D::Quad2D(){
        float vertices[] = { 
            // pos     //uv texture 
            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f,  1.0f,  1.0f, 1.0f,  // top-right
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right

            -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
            1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f,  0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &m_vertex_array);
        glBindVertexArray(m_vertex_array);

        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    }
    Quad2D::~Quad2D(){

    }
}
