#include "Entity.h"

namespace ECS {
    Player::Player(glm::vec2 pos, glm::u32 id, MovementKeys keys) : pos(pos), id(id), texture_key(PrimaryKeys::Player), keys(keys){
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
    void Player::tick(float delta_ms){
        this->pos += this->dir * this->speed * delta_ms;
    }
    Bush::Bush(glm::vec2 pos, glm::u32 id) : pos(pos), id(id), texture_key(PrimaryKeys::Bush){
	}
}
