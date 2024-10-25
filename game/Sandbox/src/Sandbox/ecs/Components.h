#pragma once

#include <functional>
#include <mutex>
#include <atomic>

#include <entt.hpp>


#include "Syris/utils/FastFunction.hpp"
#include "Syris/renderer/Texture.h"
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/utils/EngineTime.hpp"
#include "Syris/materials/MaterialManager.hpp"
#include "CollectableManager.hpp"

namespace Sandbox::ecs {
    struct SMaterialID{
        Syris::MaterialManager::MaterialID material_id;
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
        void set(T& data){
            m_mutex.lock();
            m_data = data;
            m_mutex.unlock();
        }
        void set(T data){
            m_mutex.lock();
            m_data = data;
            m_mutex.unlock();
        }
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
        Syris::texture::Rectangle2D rect;
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
        enum class TileType : glm::u8{
            Grass,
            Water
        };
        TileType type;
        std::mutex mutex;
        bool in_use = false;
        CTile(TileType type): type(type){}
        CTile(const CTile& ref): type(ref.type){}
    };

    struct CAnimated{
        std::function<void(entt::registry&, entt::entity, Syris::engine_time::Time time, Syris::texture::Texture2DBundle& texture)> animate; 
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
        Syris::FastFunction<void, Syris::EntityManager&, entt::entity, const Syris::engine_time::Time&> tick;
    };
    struct CComposition{
        float life_matter;
        float regen_per_ms;
    };
    struct CSource{
        entt::entity source;
        float life_matter_consume_per_ms;
    };
    struct CMaterialID{
        Syris::MaterialManager::MaterialID id;
    };
    struct CCollectable{
        CollectableManager::ID id;
        float current_life_matter;
        uint8_t current_stage;
        //Syris::FastFunction<void, Syris::EntityManager&, entt::entity, const Syris::engine_time::Time&> tick;
    };
}
