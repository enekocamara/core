#include "Entity.h"

namespace ECS {
    namespace behaviours {
        template<typename T>
        concept Movement = requires(T t){
            t.pos;
            t.dir;
            t.speed;
        };
        template<Movement T>
        static void move(float delta_time_ms, T& data){
            data.pos += data.dir * data.speed * delta_time_ms;
        }
    }

}
