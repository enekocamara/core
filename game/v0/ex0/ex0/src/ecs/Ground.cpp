#include "Ground.h"
namespace ECS{
    Ground::Ground(glm::vec2 pos, glm::u32 id) : pos(pos), id(id), texture(Ground::getTextureBundleDefault()){
    }
    void Ground::run_tick(engine_time::Time time) {
        this->behaviours.run_tick(time, *this);
    }
    IEntityTick<Ground>& Ground::pushTickBehaviour(tick_function<Ground> fn){
        this->behaviours.pushTickBehaviour(fn);
        return *this;
    }
    IEntityTick<Ground>& Ground::insertTickBehaviour(std::span<tick_function<Ground>> fns) {
        this->behaviours.insertTickBehaviour(fns);
        return *this;

    }
    TextureBundle Ground::getTextureBundleDefault(){
        return ECS::TextureBundle{
            .src = ECS::TextureSrc{
                .key = ECS::TextureKeys::Grass,
                .rect = Rectangle {
                   .x = config::tile_size,
                   .y = config::tile_size * 5,
                   .width = config::tile_size,
                   .height = config::tile_size,
                }
            },
            .size = {config::render_tile_size, config::render_tile_size},
            .color = RAYWHITE,
            .rotation = 0
        };
    }
}
