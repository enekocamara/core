#pragma once
#include <array>

#include "../../Libs.h"
#include "../Texture.h"
#include "../../EngineTime.h"
namespace Syris::renderer{
    enum class GameEngineState{
        Init,
        Running,
        Stop,
        WindowClosed
    };

    struct Quad2D{
        Quad2D();
        ~Quad2D();
        GLuint m_vertex_array;
        GLuint m_vertex_buffer;
        
    };
    struct Triangle2D{
        Triangle2D();
        ~Triangle2D();
        GLuint m_vertex_array;
        GLuint m_vertex_buffer;
    };

    class Renderer2D{
        public:
            Renderer2D(entt::registry& registry);
            ~Renderer2D();

            //render api
            void render_quad(int program, Quad2D quad, glm::mat4 model,glm::mat4 projection_view, texture::Texture2D texture, texture::Rectangle2D src, glm::vec3 color);
            void clear_window(glm::vec3 color);
        private:
            entt::registry& m_registry;
            //entt::entity player;
            std::optional<entt::entity> interactable = std::nullopt;
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            engine_time::Time time;
            glm::ivec2 screen_size;
    };
}
