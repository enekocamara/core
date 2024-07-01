#include "Entity.h"

namespace ECS {
    Player::Player(glm::vec2 initPos, glm::u32 id, Color color, MovementKeys keys) : pos(initPos), id(id), color(color), keys(keys){
        std::cout << "player constructed at pos" << initPos << '\n'; 
	}
    void Player::handleKeys(std::array<bool,MAX_NUMBER_KEYS>& keys, std::chrono::high_resolution_clock::duration delta) {
        if (keys[this->keys.up])
            pos.y -= 1 * duration;
        if (keys[this->keys.down])
            pos.y += 1;
        if (keys[this->keys.right])
            pos.x += 1;
        if (keys[this->keys.left])
            pos.x -= 1;
    }
}
