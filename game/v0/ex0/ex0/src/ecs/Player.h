#pragma once
#include "Entity.h"
#include <span>
namespace ECS {
    class Player : public IEntityTexture, public IEntityKey, public IEntityTick<Player> {
        public:
            Player(glm::vec2 pos, glm::u32 id, MovementKeys keys);
            ~Player(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            TextureKeys getTextureKey()  const override {return this->texture.key;}

            void run_tick(float delta_ms) override;
            IEntityTick<Player>& pushTickBehaviour(tick_function<Player> fn) override;
            IEntityTick<Player>& insertTickBehaviour(std::span<tick_function<Player>> fns) override;

            glm::vec2 dir;//normalized
            glm::vec2 pos;
            float speed = 1;// unit/ms
        private:
            void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, float delta_ms) override;
            glm::u32 id;
            TextureBundle texture;
            MovementKeys keys;
            TickBehaviour<Player> behaviours;
    };
}
