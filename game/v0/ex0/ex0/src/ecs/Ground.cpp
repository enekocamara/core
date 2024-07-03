#include "Ground.h"
namespace ECS{
    Ground::Ground(glm::vec2 pos, glm::u32 id) : pos(pos), id(id), texture_key(PrimaryKeys::Grass){
    }
    void Ground::run_tick(float delta_ms){
        this->behaviours.run_tick(delta_ms, *this);
    }
    IEntityTick<Ground>& Ground::pushTickBehaviour(tick_function<Ground> fn){
        this->behaviours.pushTickBehaviour(fn);
        return *this;
    }
    IEntityTick<Ground>& Ground::insertTickBehaviour(std::span<tick_function<Ground>> fns) {
        this->behaviours.insertTickBehaviour(fns);
        return *this;

    }
}
