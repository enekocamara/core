#pragma once
#include "../Libs.h"
#include <entt.hpp>
#include "Components.h"
#include "Bush.h"
#include <cstdlib>
#include <ctime>
namespace ecs {
    namespace Tile{
        static void tick(entt::registry& registry, entt::entity entity, engine_time::Time time){
            auto[c_composition, c_position, c_tile] = registry.get<CComposition, CPosition, CTile>(entity);
            c_composition.life_matter += time.delta_time_ms * c_composition.regen_per_ms;
            if (c_composition.life_matter > 80 &&  c_tile.type == ecs::CTile::TileType::Grass){
                ecs::Bush::newBush(c_position.pos, ecs::Bush::defaultTextureBundle(),registry, entity);
                c_composition.life_matter -= 40;
            }
        }

        inline entt::entity newTile(glm::vec2 pos, Texture2DBundle texture, entt::registry& registry, ecs::CTile::TileType type){
            entt::entity tile = registry.create();
            registry.emplace<CTile>(tile,type);
            registry.emplace<CPosition>(tile, pos);
            registry.emplace<CTexture>(tile, texture);
            registry.emplace<CComposition>(tile, float(rand() % 50), 0.01f);
            registry.emplace<CTick>(tile, tick);
            return tile;
        }
        inline Texture2DBundle defaultTextureBundle(){
            return Texture2DBundle{
                .src = TextureSrc{
                    .key = TextureKeys::Grass,
                        .rect = textures::grass_0
                },
                    .size = {(float)config::render_tile_size, (float)config::render_tile_size},
             ///       .color = RAYWHITE,
             ///       .rotation = 0
            };
        }
   };
}
