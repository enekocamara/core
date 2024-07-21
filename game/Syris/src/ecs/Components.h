#pragma once
#include "EntityComponentSystem.h"
#include "../renderer/Texture.h"
#include "Entity.h"
#include <functional>
namespace ecs {
    struct CTexture{
        Texture2DBundle texture;
        CTexture(Texture2DBundle bundle): texture(bundle){}
        CTexture(const CTexture& ref): texture(ref.texture){}
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
    };
    struct CDir { glm::fvec2 value; };
    struct CSpeed { float value; };
    struct CAcceleration { glm::fvec2 value; };
    struct CTile{    
        enum class TileType : glm::u8{
            Grass,
            Water
        };
        TileType type;
        CTile(TileType type): type(type){}
        CTile(const CTile& ref): type(ref.type){}
    };

    struct CAnimated{
        std::function<void(entt::registry&, entt::entity, engine_time::Time time, Texture2DBundle& texture)> animate; 
    };
    struct CInteractable{
        enum class InteractionType{
            Gader,
        };
        InteractionType interaction_type;
        const char * message;
        std::function<bool(entt::registry&, entt::entity)> can_interact;
        std::function<void(entt::registry&, entt::entity)> interact;

        CInteractable(const char *message, InteractionType type, std::function<bool(entt::registry&, entt::entity)> can_interact,std::function<void(entt::registry&, entt::entity)> interact ):message(message), interaction_type(type), can_interact(can_interact), interact(interact){
        }
        CInteractable(const CInteractable& ref): message(ref.message),interaction_type(ref.interaction_type), can_interact(ref.can_interact), interact(ref.interact){
        }
    };
    struct CTick{
        std::function<void(entt::registry&, entt::entity, engine_time::Time time)> tick;
    };
    struct CComposition{
        float life_matter;
        float regen_per_ms;
    };
    struct CSource{
        entt::entity source;
        float life_matter_consume_per_ms;
    };
}
