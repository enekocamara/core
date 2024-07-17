#pragma once
#include "../Libs.h"
#include <array>
#include "../renderer/Texture.h"
#include <functional>
#include "../EngineTime.h"
namespace ecs {
    enum class EntityHeightFlag{
        Floor,
        Surface,
        Max
    };

    template<typename T>
    using tick_function = std::function<void(engine_time::Time time, T& data)>;

    static constexpr glm::u32 player_id_undefined = std::numeric_limits<glm::u32>::max();

    struct MovementKeys{
        int up;
        int down;
        int left;
        int right;
    };

    class IRunTickBehaviour{
        public:
            IRunTickBehaviour(){};
            virtual ~IRunTickBehaviour() = default;
            virtual glm::u32 getId() const = 0;
            virtual void run_tick(engine_time::Time time) = 0;
    };

    template<typename T>
    class ISetTickBehaviour{
        public:
            ISetTickBehaviour(){};
            virtual ~ISetTickBehaviour() = default;
            virtual ISetTickBehaviour& pushTickBehaviour(tick_function<T> fn) = 0;
            virtual ISetTickBehaviour& insertTickBehaviour(std::span<tick_function<T>> fns) = 0;
    };
    template<typename T>
    class IEntityTick : public IRunTickBehaviour,  public ISetTickBehaviour<T>{
        public:
            IEntityTick(){};
            virtual ~IEntityTick() = default;
    };

    class IEntity{
        public:
            IEntity() {}
            virtual ~IEntity() = default;
            virtual glm::u32 getId() const = 0;

    };

    class IEntityTexture : public IEntity{
        public:
            IEntityTexture() {}
            virtual ~IEntityTexture() = default;

            virtual glm::vec2 getPos() const = 0;
            virtual TextureKeys getTextureKey() const = 0;
            virtual TextureBundle getTextureBundle() const = 0;
    };

    class IEntityKey: public IEntity{
        public:
            IEntityKey() {}
            virtual ~IEntityKey() = default;

            virtual void handleKeys(std::array<bool, MAX_NUMBER_KEYS>& keys, float delta) = 0;
    };

    template<typename T>
    class TickBehaviour{
        public:
            TickBehaviour(){}
            ~TickBehaviour(){}
            void pushTickBehaviour(tick_function<T> fn);
            void insertTickBehaviour(std::span<tick_function<T>> fns);
            void run_tick(engine_time::Time time, T& data);
        private:
            std::vector<tick_function<T>> functions;
    };
    template<typename T>
    void TickBehaviour<T>::pushTickBehaviour(tick_function<T> fn){
        functions.push_back(fn);
    }
    template<typename T>
    void TickBehaviour<T>::insertTickBehaviour(std::span<tick_function<T>> fns){
        this->functions.insert(this->functions.end(), fns.begin(), fns.end());
    }
    template<typename T>
    void TickBehaviour<T>::run_tick(engine_time::Time time, T& data){
        for (auto function : this->functions){
            function(time, data);
        }
    }

}
