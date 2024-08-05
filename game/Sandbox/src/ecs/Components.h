#pragma once
#include <entt.hpp>
#include "Syris/renderer/Texture.h"
#include "Syris/utils/EngineTime.h"
#include <functional>
namespace Sandbox::ecs {
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
        std::function<void(entt::registry&, entt::entity, Syris::engine_time::Time time, Syris::texture::Texture2DBundle& texture)> animate; 
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
        std::function<void(entt::registry&, entt::entity, Syris::engine_time::Time time)> tick;
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
