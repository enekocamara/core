#include "Entity.h"
namespace ECS {
    class Mushroom : public IEntityTexture, public IEntityTick<Mushroom> {
        public:
            Mushroom(glm::vec2 pos, glm::u32 id);
            ~Mushroom(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            TextureKeys getTextureKey()  const override {return this->texture.src.key;}
            void run_tick(engine_time::Time time) override {};
        private:
            glm::vec2 pos;
            glm::u32 id;
            TextureBundle texture;
    };
}
