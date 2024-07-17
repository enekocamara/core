#pragma once
#include "../../Libs.h"
#include "../Texture.h"
#include "../../EngineTime.h"
#include <array>
#include "../../scene/Scene.hpp"
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
    class Renderer2D{
        public:
            Renderer2D(scene::Scene& scene, entt::registry& registry);
            ~Renderer2D();
            void renderFrame();
        private:
            void initTextures();
            void logic();
            void handleKeys();
            void populateTextureAtlas();
            template< bool debug,typename T>
            void draw_group(T& render_group);
            void draw_interact();
            void handle_interactable();
            void handle_tick();

            entt::registry& m_registry;
            //entt::entity player;
            std::optional<entt::entity> interactable = std::nullopt;
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            engine_time::Time time;
            glm::ivec2 screen_size;
            ecs::TextureAtlas textureAtlas;
            scene::Scene& scene;
///            Camera2D camera;
///            Music backgroundMusic;
    };
}
