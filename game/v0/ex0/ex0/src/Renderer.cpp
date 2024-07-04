#include "Renderer.h"
#include "ecs/Bush.h"
#include "ecs/Ground.h"
#include "ecs/Mushroom.h"
#include "ecs/Player.h"
namespace ge {

    template<glm::u32 x, glm::u32 y>
    std::array<ECS::IEntity *, x * y> make_bushes(glm::vec2 top_left, float padding){
        std::array<ECS::IEntity *, x * y> bushes;
        for (glm::u32 j = 0; j < y; j++){
            for (glm::u32 i = 0; i < x; i++){
                bushes[j * y + i] = new ECS::Bush({i * padding + top_left.x, j * padding + top_left.y}, genId());
            }
        }
        return bushes;
    }

    Renderer::Renderer(){
        //window
        this->time.init_time = std::chrono::high_resolution_clock::now(),
        InitWindow(0,0, "example");
        this->screen_size = {GetScreenWidth(), GetScreenHeight()};
        this->textureAtlas.init();
        //player
        ECS::MovementKeys player_keys = {KEY_W, KEY_S, KEY_A, KEY_D};
        ECS::Player* player = new ECS::Player({0,0}, genId(), player_keys, ECS::Player::getTextureBundleDefault());
        this->entityECS.setPlayerId(player->getId());
        this->entityECS.pushEntity(player, ECS::EntityHeightFlag::Surface);
        this->entityECS.pushKeyEntity(player);
        this->entityECS.pushRunTickEntity(player);

        //bushes
        //auto bushes = make_bushes<10, 10>({0,0}, 100);
        //this->entities.insert(this->entities.end(),bushes.begin(), bushes.end());
        //generate ground
        this->map_config = MapConfig{
            .num_tiles_x = 100,
            .num_tiles_y = 100
        };
        this->generateGround();
        //textures
        //audio
        InitAudioDevice();
        this->backgroundMusic = LoadMusicStream("sound/music/Kokia-Fukurou.mp3");
        //camera
        glm::vec2 player_pos = {0,0};
        ECS::TextureBundle player_texture =  this->entityECS.getPlayer()->getTextureBundle();
        this->camera = Camera2D{
            .offset =  {this->screen_size.x / 2.0f, this->screen_size.y / 2.0f},
            .target = {player_pos.x - player_texture.size.x / 2.0f, player_pos.y - player_texture.size.y / 2.0f},
            .rotation = 0,
            .zoom = 1
        };
    }
    Renderer::~Renderer(){
        this->state = GameEngineState::WindowClosed;
        UnloadMusicStream(this->backgroundMusic);
        CloseAudioDevice();
        CloseWindow();
    }
    void Renderer::run(){
        this->time.last_frame = std::chrono::high_resolution_clock::now();
        this->state = GameEngineState::Running;
        PlayMusicStream(this->backgroundMusic);
        while (!WindowShouldClose()) {
            UpdateMusicStream(this->backgroundMusic);
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            this->time.delta_time_ms = std::chrono::duration<float, std::milli>(now - time.last_frame).count();
            if (this->state != GameEngineState::Running)
                break;
            this->logic();
            this->renderFrame();
            this->time.last_frame = now;
        }
    }
    void Renderer::logic(){
        this->handleKeys();
        for (auto entity : this->entityECS.getRunTickEntities()){
            entity->run_tick(this->time);
        }
        
    }
    void Renderer::stop(){
        std::cout << "game stopped\n";
        this->state = GameEngineState::Stop;
    }
    void Renderer::renderFrame(){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        auto player = this->entityECS.getPlayer();
        ECS::TextureBundle player_texture = this->entityECS.getPlayer()->getTextureBundle();
        glm::vec2 player_pos = player->getPos();
        
        this->camera.target = {player_pos.x + player_texture.size.x / 2.0f, player_pos.y + player_texture.size.y / 2.0f};
        BeginMode2D(this->camera);
        std::cout << "rendering: " <<  player_texture.src.rect.x <<" " << player_texture.src.rect.width << "\n";
        constexpr bool render_debug = false;
        for(auto entities : this->entityECS.getEntities()){
            for (auto entity : entities){
                Texture texture = this->textureAtlas.getTexture(entity->getTextureKey());
                if (texture.id == 0){
                    std::cout << "texture uninitialized\n";
                    exit(1);
                }
                ECS::TextureBundle bundle = entity->getTextureBundle();
                DrawTexturePro(
                        texture,
                        bundle.src.rect,
                        Rectangle{
                        .x = entity->getPos().x,
                        .y = entity->getPos().y,
                        .width = bundle.size.x,
                        .height = bundle.size.y,
                        },{0,0},0,bundle.color
                        );
                if constexpr (render_debug)
                    DrawRectangle(entity->getPos().x + 10, entity->getPos().y + 10, bundle.size.x - 20, bundle.size.y - 20, RED);
            }
        }
        EndMode2D();
        DrawFPS(0, 0);
        EndDrawing();
    }
    void Renderer::handleKeys(){   
        for (int i = 1; i < MAX_NUMBER_KEYS; i++){
            if (IsKeyPressed(i))
                this->keys[i] = true;
            if (IsKeyReleased(i))
                this->keys[i] = false;
        }
        for (auto key_entity : this->entityECS.getKeyEntities()){
            key_entity->handleKeys(keys, this->time.delta_time_ms);
        }
    }
    void Renderer::generateGround(){
        glm::vec2 origin = {this->map_config.num_tiles_x * config::render_tile_size / -2.0f,this->map_config.num_tiles_y * config::render_tile_size / -2.0f};
        std::cout << "origin: " << origin << '\n';
        for (size_t y = 0; y < this->map_config.num_tiles_y; y++){
            for (size_t x = 0; x < this->map_config.num_tiles_x; x++){
                glm::vec2 pos = {origin + glm::vec2(x * config::render_tile_size, y * config::render_tile_size)};
                this->entityECS.pushEntity(new ECS::Ground(pos, genId()), ECS::EntityHeightFlag::Floor);
            }
        }
    }
}
