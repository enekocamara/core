#pragma once
#include "EntityComponentSystem.h"
#include "../Texture.h"
#include <functional>
namespace ecs {
    struct CTexture{
        TextureBundle texture;
        CTexture(TextureBundle bundle): texture(bundle){}
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

    struct CLife { float value; };
    struct CAnimated{
        std::function<void(entt::registry&, entt::entity, engine_time::Time time, TextureBundle& texture)> animate; 
    };
}
