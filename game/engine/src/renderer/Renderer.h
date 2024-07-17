#pragma once
#include "../Libs.h"
#include "Texture.h"
#include "../EngineTime.h"
#include <array>
namespace ge{

    inline glm::u32 genId(){
        static glm::u32 current_id = 0;
        glm::u32 id = current_id;
        current_id += 1;
        return id;
    }
    enum class GameEngineState{
        Init,
        Running,
        Stop,
        WindowClosed
    };
    class Renderer{
        public:
            Renderer();
            void run();
            void stop();
            ~Renderer();
        private:
            void initTextures();
            void renderFrame();
            void logic();
            void handleKeys();
            void populateTextureAtlas();
            template< bool debug,typename T>
            void draw_group(T& render_group);
            void draw_interact();
            void handle_interactable();
            void handle_tick();

            entt::registry ecs_registry;
            entt::entity player;
            std::optional<entt::entity> interactable = std::nullopt;
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            engine_time::Time time;
            glm::ivec2 screen_size;
            ecs::TextureAtlas textureAtlas;
///            Camera2D camera;
///            Music backgroundMusic;
            static constexpr config::MapConfig map_config = config::MapConfig{
                .num_tiles_x = 100,
                .num_tiles_y = 100
            };
    };
}
