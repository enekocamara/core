#include "Renderer.h"
namespace ge {
    Renderer::Renderer(){
        this->time.init_time = std::chrono::high_resolution_clock::now(),
        InitWindow(800, 450, "example");
        Color player_color = Color{0,0,0,255};
        ECS::MovementKeys player_keys = {KEY_W, KEY_S, KEY_A, KEY_D};
        ECS::Player* player = new ECS::Player({0,0}, genId(), player_color, player_keys);
        this->entities.push_back(player);
        this->key_entities.push_back(player);
    }
    Renderer::~Renderer(){
        CloseWindow();
        this->state = GameEngineState::WindowClosed;
        for (auto entity : this->entities){
            delete entity;
        }
        this->entities.clear();
    }
    void Renderer::run(){
        this->time.last_frame = std::chrono::high_resolution_clock::now();
        this->state = GameEngineState::Running;
        while (!WindowShouldClose()) {
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            this->time.delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(time.last_frame - now);
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
        DrawFPS(0, 0);
        for(auto entity : this->entities){
            handleKeys();
            DrawRectangle((int)entity->getPos().x, (int)entity->getPos().y, 5, 5, entity->getCol());
        }
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
            key_entity->handleKeys(keys, this->time.delta_time);
        }
    }
}
