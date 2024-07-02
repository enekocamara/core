#include "Renderer.h"
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
        //player
        ECS::MovementKeys player_keys = {KEY_W, KEY_S, KEY_A, KEY_D};
        ECS::Player* player = new ECS::Player({0,0}, genId(), player_keys);
        this->entities.push_back(player);
        this->key_entities.push_back(player);
        //bushes
        auto bushes = make_bushes<10, 10>({0,0}, 100);
        this->entities.insert(this->entities.end(),bushes.begin(), bushes.end());
        //textures
        this->populateTextureAtlas();
        //audio
        InitAudioDevice();
        this->backgroundMusic = LoadMusicStream("sound/music/Kokia-Fukurou.mp3");
        //camera
        glm::vec2 player_pos = {0,0};
        ECS::TextureInfo info =  this->textureAtlas.getTexture(ECS::PrimaryKeys::Player);
        this->camera = Camera2D{
            .offset =  {this->screen_size.x / 2.0f, this->screen_size.y / 2.0f},
            .target = {player_pos.x - info.dimmensions.dst_width / 2.0f, player_pos.y - info.dimmensions.dst_height / 2.0f},
            .rotation = 0,
            .zoom = 1
        };
    }
    Renderer::~Renderer(){
        this->state = GameEngineState::WindowClosed;
        for (auto entity : this->entities){
            delete entity;
        }
        this->entities.clear();
        UnloadMusicStream(this->backgroundMusic);
        CloseAudioDevice();
        CloseWindow();
    }
    void Renderer::run(){
        this->time.last_frame = std::chrono::high_resolution_clock::now();
        this->state = GameEngineState::Running;
        PlayMusicStream(this->backgroundMusic);
        while (!WindowShouldClose()) {
            std::cout << "player position [" << this->entities[0]->getPos() << "]\n";
            UpdateMusicStream(this->backgroundMusic);
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            this->time.delta_time_ms = std::chrono::duration<float, std::milli>(now - time.last_frame).count();
            if (this->state != GameEngineState::Running)
                break;
            this->renderFrame();
            this->time.last_frame = now;
        }
    }
    void Renderer::stop(){
        std::cout << "game stopped\n";
        this->state = GameEngineState::Stop;
    }
    void Renderer::renderFrame(){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        
        ECS::TextureInfo player_texture_info = this->textureAtlas.getTexture(this->entities[0]->getTextureKey());
        glm::vec2 player_pos = this->entities[0]->getPos();
        //camera
        handleKeys();

        bool player = true;
        for(auto entity : this->entities){
            entity->tick(this->time.delta_time_ms);
            if (player){
                this->camera.target = {player_pos.x + player_texture_info.dimmensions.dst_width / 2.0f, player_pos.y + player_texture_info.dimmensions.dst_height / 2.0f};
                DrawFPS(0, 0);
                BeginMode2D(this->camera);
            }
            auto info = this->textureAtlas.getTexture(entity->getTextureKey());
            DrawTexturePro(
                    info.texture,
                    info.dimmensions.src_rect,
                    Rectangle{
                        .x = entity->getPos().x,
                        .y = entity->getPos().y,
                        .width = info.dimmensions.dst_width,
                        .height = info.dimmensions.dst_height,
                    }, {0,0}, 0, RAYWHITE
            );
            player = false;
        }
        EndMode2D();
        EndDrawing();
    }
    void Renderer::handleKeys(){   
        for (int i = 1; i < MAX_NUMBER_KEYS; i++){
            if (IsKeyPressed(i))
                this->keys[i] = true;
            if (IsKeyReleased(i))
                this->keys[i] = false;
        }
        for (auto key_entity : this->key_entities){
            key_entity->handleKeys(keys, this->time.delta_time_ms);
        }
    }
    void Renderer::populateTextureAtlas(){
        Texture2D texture_grass = LoadTexture("textures/sprites/Tilesets/Grass.png");
        Texture2D texture_player = LoadTexture("textures/sprites/characters/BasicCharakterSpritesheet.png");
        Texture2D texture_bush = LoadTexture("textures/sprites/Objects/Basic_Grass_Biom_things.png");
        ECS::TextureInfo player_texture_info = ECS::TextureInfo{
            .dimmensions = ECS::TextureDimmensions{
                .src_rect = Rectangle{
                    .x = 0,
                    .y = 0,
                    .width = 48,
                    .height = 48
                },
                .dst_width = 48,
                .dst_height = 48,
            },
            .texture = texture_player
        };
        ECS::TextureInfo grass_texture_info = ECS::TextureInfo{
            .dimmensions = ECS::TextureDimmensions{
                .src_rect = Rectangle{
                    .x = 0,
                    .y = 0,
                    .width = 48,
                    .height = 48
                },
                .dst_width = 100,
                .dst_height = 100,
            },
            .texture = texture_grass
        };
        ECS::TextureInfo bush_texture_info = ECS::TextureInfo{
            .dimmensions = ECS::TextureDimmensions{
                .src_rect = Rectangle{
                    .x = 0,
                    .y = 0,
                    .width = 48,
                    .height = 48
                },
                .dst_width = 100,
                .dst_height = 100,
            },
            .texture = texture_bush
        };
        this->textureAtlas.pushTexture(ECS::PrimaryKeys::Player, player_texture_info)
            .pushTexture(ECS::PrimaryKeys::Bush, bush_texture_info)
            .pushTexture(ECS::PrimaryKeys::Grass, grass_texture_info);
    }
}
