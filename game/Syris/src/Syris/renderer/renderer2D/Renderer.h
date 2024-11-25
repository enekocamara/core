#pragma once
#include <array>
#include <entt.hpp>

#include "Syris/texture/Texture.hpp"
#include "Syris/utils/EngineTime.hpp"
#include "Syris/renderAPI/renderApi.h"

namespace Syris::renderer{
    enum class GameEngineState{
        Init,
        Running,
        Stop,
        WindowClosed
    };

    class Renderer2D{
        public:
            Renderer2D();
            ~Renderer2D();

            //render api
            void draw_quad(int program, renderAPI::Quad2D* quad, glm::mat4 model,glm::mat4 projection_view, Texture2D& texture, Rectangle2D src, glm::vec3 color);
            void clear_window(glm::vec3 color);
        private:
            //entt::entity player;
            std::optional<entt::entity> interactable = std::nullopt;
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            //engine_time::Time time;
            glm::ivec2 screen_size;
    };
}
