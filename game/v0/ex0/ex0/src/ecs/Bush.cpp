#include "Bush.h"

namespace ECS {
    Bush::Bush(glm::vec2 pos, glm::u32 id) : pos(pos), id(id), texture_key(PrimaryKeys::Bush){
	}
    void Bush::run_tick(float delta_ms){
        this->behaviours.run_tick(delta_ms, *this);
    }
    IEntityTick<Bush>& Bush::pushTickBehaviour(tick_function<Bush> fn){
        this->behaviours.pushTickBehaviour(fn);
        return *this;
    }
    IEntityTick<Bush>& Bush::insertTickBehaviour(std::span<tick_function<Bush>> fns) {
        this->behaviours.insertTickBehaviour(fns);
        return *this;

    }
}
