#pragma once
#include <entt.hpp>
#include <cstdlib>
#include <ctime>

#include "Syris/Libs.h"
#include "Components.h"
#include "Bush.h"
namespace Sandbox::ecs::Tile {
    static void tick(entt::registry& registry, entt::entity entity, Syris::engine_time::Time time){
        auto[c_composition, c_position, c_tile] = registry.get<CComposition, CPosition, CTile>(entity);
        c_composition.life_matter += time.delta_time_ms * c_composition.regen_per_ms;
        if (c_composition.life_matter > 80 &&  c_tile.type == ecs::CTile::TileType::Grass){
            ecs::Bush::newBush(c_position.pos, ecs::Bush::defaultTextureBundle(),registry, entity);
            c_composition.life_matter -= 40;
        }
    }

    inline entt::entity newTile(glm::vec2 pos, Syris::texture::Texture2DBundle texture, entt::registry& registry, ecs::CTile::TileType type){
        entt::entity tile = registry.create();
        registry.emplace<CTile>(tile,type);
        registry.emplace<CPosition>(tile, pos);
        registry.emplace<CTexture>(tile, texture.src);
        registry.emplace<CComposition>(tile, float(rand() % 50), 0.01f);
        registry.emplace<CTick>(tile, tick);
        return tile;
    }
    
    inline Syris::texture::Texture2DBundle defaultTextureBundle(){
        return Syris::texture::Texture2DBundle{
            .src = texture::atlas::grass_0,
            .size = {(float)Syris::config::render_tile_size, (float)Syris::config::render_tile_size},
                ///       .color = RAYWHITE,
                ///       .rotation = 0
        };
    }
}
