#include "Bush.h"

namespace ECS {
    Bush::Bush(glm::vec2 pos, glm::u32 id) : pos(pos), id(id), texture(Bush::getTextureBundleDefault()){
	}
    void Bush::run_tick(engine_time::Time time){
        this->behaviours.run_tick(time, *this);
    }
    IEntityTick<Bush>& Bush::pushTickBehaviour(tick_function<Bush> fn){
        this->behaviours.pushTickBehaviour(fn);
        return *this;
    }
    IEntityTick<Bush>& Bush::insertTickBehaviour(std::span<tick_function<Bush>> fns) {
        this->behaviours.insertTickBehaviour(fns);
        return *this;

    }
    TextureBundle Bush::getTextureBundleDefault(){
        std::cout << "TODO\n";
        exit(1);
        return ECS::TextureBundle{};
    }
}
