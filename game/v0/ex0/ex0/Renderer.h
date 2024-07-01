#include "Libs.h"
#include "Entity.h"
#include <vector>
#include <array>
namespace ge{
    typedef std::chrono::high_resolution_clock::time_point  ge_time_point;
    struct Time{
        ge_time_point init_time;
        std::chrono::milliseconds running;
        ge_time_point last_frame;
        std::chrono::milliseconds delta_time;
    };
    inline glm::u32 genId(){
        static glm::u32 current_id = 0;
        glm::u32 id = current_id;
        current_id += 1;
        return id;
    }
    enum GameEngineState{
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
            void renderFrame();
            void handleKeys();
            std::vector<ECS::IEntity*> entities;//main memory space
            std::vector<ECS::IKeyEntity*> key_entities;//main memory space
            GameEngineState state = GameEngineState::Init;
            std::array<bool, 249> keys = {false};
            Time time;
    };
}
