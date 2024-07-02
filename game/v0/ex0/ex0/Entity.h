#pragma once
#include "Libs.h"
#include <array>
#include "Texture.h"
namespace ECS {

    struct MovementKeys{
        int up;
        int down;
        int left;
        int right;
    };


    class IEntity{
        public:
            IEntity() {}
            virtual ~IEntity() = default;

            virtual glm::u32 getId() const = 0;
            virtual glm::vec2 getPos() const = 0;
            virtual PrimaryKeys getTextureKey() const = 0;
            virtual void tick(float delta_ms) = 0;
    };
    class IKeyEntity{
        public:
            IKeyEntity() {}
            virtual ~IKeyEntity() = default;

            virtual void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, float delta) = 0;
    };

    class Player : public IEntity, public IKeyEntity {
        public:
            Player(glm::vec2 pos, glm::u32 id, MovementKeys keys);
            ~Player(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            PrimaryKeys getTextureKey()  const override {return this->texture_key;}
            void tick(float delta_ms) override;
        private:
            void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, float delta_ms) override;
            glm::vec2 dir;//normalized
            glm::vec2 pos;
            float speed = 1;// unit/ms
            glm::u32 id;
            PrimaryKeys texture_key;
            MovementKeys keys;
    };

    class Bush : public IEntity {
        public:
            Bush(glm::vec2 pos, glm::u32 id);
            ~Bush(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            PrimaryKeys getTextureKey()  const override {return this->texture_key;}
            void tick(float delta_ms) override {};
        private:
            glm::vec2 pos;
            glm::u32 id;
            PrimaryKeys texture_key;
    };
}


