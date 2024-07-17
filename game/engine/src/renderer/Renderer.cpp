#include "Renderer.h"
#include "../ecs/Player.h"
#include "../ecs/Mushroom.h"
#include "../world_generator/world_generator.hpp"
namespace ge {
    Renderer::Renderer(){
        //window
        this->time.init_time = std::chrono::high_resolution_clock::now(),
        ///InitWindow(0,0, "example");
        ///this->screen_size = {GetScreenWidth() - 400, GetScreenHeight() - 400};
        ///SetWindowSize(this->screen_size.x, this->screen_size.y);
        this->textureAtlas.init();
        //player
        ///ecs::MovementKeys player_keys = {KEY_W, KEY_S, KEY_A, KEY_D};
        ///this->player =  ecs::Player::newPlayer({0,0}, player_keys, ecs::Player::defaultTextureBundle(), this->ecs_registry);

        world_generator::generateGround<this->map_config>(this->ecs_registry);
        
        //audio
        ///InitAudioDevice();
        
        ///this->backgroundMusic = LoadMusicStream("sound/music/Kokia-Fukurou.mp3");

        //camera
        ///ecs::TextureBundle player_texture = this->ecs_registry.get<ecs::CTexture>(this->player).texture;
        glm::vec2 player_pos = this->ecs_registry.get<ecs::CPosition>(this->player).pos;
        /***this->camera = Camera2D{
            .offset =  {this->screen_size.x / 2.0f, this->screen_size.y / 2.0f},
            .target = {player_pos.x - player_texture.size.x / 2.0f, player_pos.y - player_texture.size.y / 2.0f},
            .rotation = 0,
            .zoom = 1.f
        };***/
    }
    Renderer::~Renderer(){
        this->state = GameEngineState::WindowClosed;
        /***UnloadMusicStream(this->backgroundMusic);
        CloseAudioDevice();
        CloseWindow();***/
    }
    void Renderer::run(){
        std::cout << "player pos " << this->ecs_registry.get<ecs::CPosition>(this->player).pos;
        std::cout << "player dir " << this->ecs_registry.get<ecs::CDir>(this->player).value;
        std::cout << "player speed " << this->ecs_registry.get<ecs::CSpeed>(this->player).value;
        this->time.last_frame = std::chrono::high_resolution_clock::now();
        this->state = GameEngineState::Running;
        ///PlayMusicStream(this->backgroundMusic);
        /****while (!WindowShouldClose()) {
            UpdateMusicStream(this->backgroundMusic);
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            this->time.delta_time_ms = std::chrono::duration<float, std::milli>(now - time.last_frame).count();
            if (this->state != GameEngineState::Running)
                break;
            this->logic();
            this->renderFrame();
            this->time.last_frame = now;
        }***/
    }
    void Renderer::logic(){
        this->handle_tick();
        this->handle_interactable();
        this->handleKeys();
        /***if (this->interactable != std::nullopt && IsKeyDown(KEY_Q)){
            this->ecs_registry.get<ecs::CInteractable>(this->interactable.value()).interact(this->ecs_registry, this->interactable.value());
        }****/
        auto key_group = this->ecs_registry.group<ecs::CKeyBinded>(entt::get<ecs::CDir, ecs::CSpeed>);
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
                
        auto movement_group = this->ecs_registry.group<ecs::CPosition>(entt::get<ecs::CSpeed, ecs::CDir>);
        for (auto entity : movement_group){
            auto[cPosition, cSpeed, cDir] = movement_group.get<ecs::CPosition, ecs::CSpeed, ecs::CDir>(entity); 
            cPosition.pos += cDir.value * (cSpeed.value * this->time.delta_time_ms / 1000.f);
        } 
        auto animated_group = this->ecs_registry.group<ecs::CAnimated>(entt::get<ecs::CTexture>);
        for (auto entity : animated_group){
            auto[cAnimated, cTexture] = animated_group.get<ecs::CAnimated, ecs::CTexture>(entity); 
            cAnimated.animate(this->ecs_registry, entity, this->time, cTexture.texture);
        } 
    }
    void Renderer::stop(){
        std::cout << "game stopped\n";
        this->state = GameEngineState::Stop;
    }
    void Renderer::renderFrame(){
        /***BeginDrawing();
        ClearBackground(RAYWHITE);

        ecs::TextureBundle player_texture = this->ecs_registry.get<ecs::CTexture>(this->player).texture;
        glm::vec2 player_pos = this->ecs_registry.get<ecs::CPosition>(this->player).pos;

        this->camera.target = {player_pos.x + player_texture.size.x / 2.0f, player_pos.y + player_texture.size.y / 2.0f};
        BeginMode2D(this->camera);

        auto render_group = this->ecs_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition, ecs::CTile>);
        this->draw_group<true>(render_group);
        auto render_group2 = this->ecs_registry.group<>(entt::get<ecs::CTexture, ecs::CPosition>, entt::exclude<ecs::CTile>);
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
        EndDrawing();***/
    }
    void Renderer::draw_interact(){
        if (this->interactable != std::nullopt){
///            DrawText(std::format("{} (q)", this->ecs_registry.get<ecs::CInteractable>(this->interactable.value()).message).c_str(), this->screen_size.x / 2 - 50,this->screen_size.y / 5 * 4, 20, WHITE);
        }
    }
    template<bool debug, typename T>
    void Renderer::draw_group(T& render_group){/***
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
        ***/
    }
    void Renderer::handle_interactable(){
        auto interact_group = this->ecs_registry.group<>(entt::get<ecs::CInteractable, ecs::CPosition>);
        glm::vec2 player_pos = this->ecs_registry.get<ecs::CPosition>(this->player).pos;

        float min_distace;
        entt::entity min_entity;
        bool first = true;
        for (auto entity : interact_group){
            glm::vec2 pos = interact_group.get<ecs::CPosition>(entity).pos;
            float distance = glm::distance(player_pos, pos);
            if ((first || distance < min_distace) && distance < 16){
                ecs::CInteractable interactable = interact_group.get<ecs::CInteractable>(entity);
                if (interactable.can_interact(this->ecs_registry, entity)){
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
    void Renderer::handle_tick(){
        auto tick_group = this->ecs_registry.group<>(entt::get<ecs::CTick>);

        for (auto entity : tick_group){
            tick_group.get<ecs::CTick>(entity).tick(this->ecs_registry, entity, this->time);
        }
    }
    void Renderer::handleKeys(){/***
        for (int i = 1; i < MAX_NUMBER_KEYS; i++){
            this->keys[i] = IsKeyDown(i);
        }
        ***/
    } 
}
