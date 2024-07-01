#pragma once
#include "Libs.h"
#include <array>
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
            virtual Color getCol() const = 0;
    };
    class IKeyEntity{
        public:
            IKeyEntity() {}
            virtual ~IKeyEntity() = default;

            virtual void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, std::chrono::milliseconds delta) = 0;
    };

    class Player : public IEntity, public IKeyEntity {
        public:
            Player(glm::vec2 initPos, glm::u32 id, Color color, MovementKeys keys);
            ~Player(){};

            glm::u32 getId() const override {return this->id;}
            glm::vec2 getPos() const override {return this->pos;}
            Color getCol() const override {return this->color;} 
        private:
            void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, std::chrono::milliseconds delta) override;
            glm::vec2 pos; 
            glm::u32 id;
            Color color;
            MovementKeys keys;
    };
}


