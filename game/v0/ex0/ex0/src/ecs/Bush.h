#pragma  once

#include "../Libs.h"
#include "Components.h"
#include <entt.hpp>

namespace ecs {
    namespace Bush{
        inline entt::entity newBush(glm::vec2 pos, TextureBundle texture,entt::registry& registry){
            entt::entity bush = registry.create();
            registry.emplace<ecs::CPosition>(bush, pos);
            registry.emplace<ecs::CTexture>(bush, texture);
            return bush;
        }
        inline TextureBundle defaultTextureBundle(){
            return TextureBundle{
                .src = TextureSrc{
                    .key = TextureKeys::Bush,
                        .rect = textures::bush_0
                },
                    .size = {(float)config::render_tile_size, (float)config::render_tile_size},
                    .color = RAYWHITE,
                    .rotation = 0
            };
        }
    };
}
