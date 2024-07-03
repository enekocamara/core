#include "Entity.h"

namespace ECS {
    class Bush : public IEntityTexture, public IEntityTick<Bush> {
        public:
            Bush(glm::vec2 pos, glm::u32 id);
            ~Bush(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            TextureKeys getTextureKey()  const override {return this->texture_key;}
            void run_tick(float delta_ms) override;
            IEntityTick<Bush>& pushTickBehaviour(tick_function<Bush> fn) override;
            IEntityTick<Bush>& insertTickBehaviour(std::span<tick_function<Bush>> fns) override;
        private:
            TickBehaviour<Bush> behaviours;
            glm::vec2 pos;
            glm::u32 id;
            TextureKeys texture_key;
    };
}
