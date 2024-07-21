#include "Player.h"
#include "Behaviours.h"
namespace ECS {
/*
    Player::Player(glm::vec2 pos, glm::u32 id, MovementKeys keys, TextureBundle textureBundle) : pos(pos), id(id), texture(textureBundle),keys(keys){
        this->behaviours.pushTickBehaviour(behaviours::move<Player>);
	}

    void Player::handleKeys(std::array<bool,MAX_NUMBER_KEYS>& keys, float delta) {
        this->dir = {0,0};
        if (keys[this->keys.up])
            this->dir.y -= 1;
        else
            this->dir.y = 0;
        if (keys[this->keys.down])
            this->dir.y += 1;
        else if (!keys[this->keys.up])
            this->dir.y = 0;
        if (keys[this->keys.right])
            this->dir.x += 1;
        else
            this->dir.x = 0;
        if (keys[this->keys.left])
            this->dir.x -= 1;
        else if (!keys[this->keys.right])
            this->dir.x = 0;
    }

    void Player::run_tick(engine_time::Time time){
        this->behaviours.run_tick(time, *this);
    }
    IEntityTick<Player>& Player::pushTickBehaviour(tick_function<Player> fn){
        this->behaviours.pushTickBehaviour(fn);
        return *this;
    }
    IEntityTick<Player>& Player::insertTickBehaviour(std::span<tick_function<Player>> fns) {
        this->behaviours.insertTickBehaviour(fns);
        return *this;

    }
    TextureBundle Player::getTextureBundleDefault(){
        return ECS::TextureBundle{
            .src = ECS::TextureSrc{
                .key = ECS::TextureKeys::Player,
                .rect = textures::player_forward_default
            },
            .size = {144, 144},
            .color = RAYWHITE,
            .rotation = 0
        };
    }
    */
}
