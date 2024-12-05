#pragma once

#include <functional>
#include <mutex>
#include <atomic>

#include <entt.hpp>


#include "Syris/utils/FastFunction.hpp"
#include "Syris/texture/Texture.hpp"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/utils/EngineTime.hpp"
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "CollectableManager.hpp"

namespace Sandbox::ecs {
    struct SER_ID{
        Syris::BatchRendererManager::BR_ID renderer_id;
    };
    template<typename T>
    class AsyncComponent{
    public:
        AsyncComponent(T value)
        {
            m_data = value;
        }
        AsyncComponent(AsyncComponent<T> &ref)
        {
            m_data = ref.get();
        }

        T get(){
            m_mutex.lock();
            T dummy = m_data;
            m_mutex.unlock();
            return dummy;
        }
        void set(const T& data){
            m_mutex.lock();
            m_data = data;
            m_mutex.unlock();
        }
        void set(T&& data){
            m_mutex.lock();
            m_data = data;
            m_mutex.unlock();
        }
        void lock(){m_mutex.lock();}
        void unlock(){m_mutex.unlock();}
    private:
        T m_data;
        std::mutex m_mutex;
    };

    
    struct MovementKeys{
        int up;
        int down;
        int left;
        int right;
    };
    struct CTexture{
        Syris::Rectangle2D rect;
    };
    struct CTileData{
        glm::f32 height = 0;
        glm::f32 humidity = 0;
        glm::f32 heat = 0;
    };
    struct CKeyBinded{
        MovementKeys keys;
        CKeyBinded(MovementKeys keys): keys(keys){}
        CKeyBinded(const CKeyBinded& ref): keys(ref.keys){}
    };
    struct CPosition {
        glm::fvec2 pos;
        CPosition(glm::fvec2 pos): pos(pos){}
        CPosition(const CPosition& ref): pos(ref.pos){}
        CPosition(): pos({0,0}){}
    };
    struct CDir { glm::fvec2 value; };
    struct CSpeed { glm::vec2 speed; };
    struct CMovementSpeed{float movement_speed;};
    struct CAcceleration { glm::fvec2 value; };
    struct CTile{
        std::size_t index;
    };

    struct CAnimated{
        std::function<void(entt::registry&, entt::entity, Syris::engine_time::Time time, Syris::Texture2DBundle& texture)> animate; 
    };
    struct CInteractable{
        enum class InteractionType{
            Gader,
        };

        CInteractable(const char *message, InteractionType type, std::function<bool(entt::registry&, entt::entity)> can_interact,std::function<void(entt::registry&, entt::entity)> interact ):message(message), interaction_type(type), can_interact(can_interact), interact(interact){
        }
        CInteractable(const CInteractable& ref): message(ref.message),interaction_type(ref.interaction_type), can_interact(ref.can_interact), interact(ref.interact){
        }
        InteractionType interaction_type;
        const char * message;
        std::function<bool(entt::registry&, entt::entity)> can_interact;
        std::function<void(entt::registry&, entt::entity)> interact;
    };
    struct CTick{
        std::function<void(Syris::EntityManager& entity_manager, entt::entity,const Syris::engine_time::Time& time)> tick;
    };
    struct CTickFast {
        using FunctionType = Syris::FastFunction<void(Syris::EntityManager&, entt::entity, const Syris::engine_time::Time&)>;
        FunctionType tick;
    };
    struct CComposition{
        float life_matter;
        float regen_per_ms;
    };
    struct CSource{
        entt::entity source;
        float life_matter_consume_per_ms;
    };
    struct CER_ID{
        Syris::BatchRendererManager::BR_ID id;
    };
    struct CCollectable{
        CollectableManager::ID id;
        float current_life_matter;
        uint8_t current_stage;
        //Syris::FastFunction<void, Syris::EntityManager&, entt::entity, const Syris::engine_time::Time&> tick;
    };
}
