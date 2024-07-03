#include "Entity.h"

namespace ECS {
    class Ground : public IEntityTexture, public IEntityTick<Ground> {
        public:
            Ground(glm::vec2 pos, glm::u32 id);
            ~Ground(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            TextureKeys getTextureKey()  const override {return this->texture_key;}

            void run_tick(float delta_ms) override;
            IEntityTick<Ground>& pushTickBehaviour(tick_function<Ground> fn) override;
            IEntityTick<Ground>& insertTickBehaviour(std::span<tick_function<Ground>> fns) override;

        private:
            TickBehaviour<Ground> behaviours;
            glm::vec2 pos;
            glm::u32 id;
            TextureKeys texture_key;
    };
}
