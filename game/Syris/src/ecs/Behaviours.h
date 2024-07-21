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
        static void move(engine_time::Time time, T& data){
            data.pos += data.dir * data.speed * time.delta_time_ms;
        }
    }

}
