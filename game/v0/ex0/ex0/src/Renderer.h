#include "Libs.h"
#include "Texture.h"
#include "ecs/Entity.h"
#include "ecs/EntityComponentSystem.h"
#include <array>
namespace ge{
    typedef std::chrono::high_resolution_clock::time_point  ge_time_point;
    struct Time{
        ge_time_point init_time;
        float running_ms;
        ge_time_point last_frame;
        float delta_time_ms;
    };

    struct MapConfig{
        size_t num_tiles_x;
        size_t num_tiles_y;
    };

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
            void generateGround();

            ECS::EntityComponentSystem entityECS;
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            Time time;
            glm::ivec2 screen_size;
            ECS::TextureAtlas textureAtlas;
            Camera2D camera;
            Music backgroundMusic;
            MapConfig map_config;
    };
}
