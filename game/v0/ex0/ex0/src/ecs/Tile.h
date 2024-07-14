#pragma once
#include "../Libs.h"
#include <entt.hpp>
#include "Components.h"

namespace ecs {
    namespace Tile{
        inline entt::entity newTile(glm::vec2 pos, TextureBundle texture, entt::registry& registry, ecs::CTile::TileType type){
            entt::entity tile = registry.create();
            registry.emplace<CTile>(tile,type);
            registry.emplace<CPosition>(tile, pos);
            registry.emplace<CTexture>(tile, texture);
            return tile;
        }
        inline TextureBundle defaultTextureBundle(){
            return TextureBundle{
                .src = TextureSrc{
                    .key = TextureKeys::Grass,
                        .rect = textures::grass_0
                },
                    .size = {(float)config::render_tile_size, (float)config::render_tile_size},
                    .color = RAYWHITE,
                    .rotation = 0
            };
        }
   };
}
