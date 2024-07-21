#pragma  once

#include "../Libs.h"
#include "Components.h"
#include <entt.hpp>
#include "../renderer/Texture.h"

namespace ecs {
    namespace Bush{
        struct CHasBerrys{
            bool value;
            std::chrono::high_resolution_clock::time_point time_collected;
            CHasBerrys(bool val) : value(val){}
        };
        static bool can_interact(entt::registry& registry, entt::entity bush){
            return  registry.get<CHasBerrys>(bush).value;
        }
        static void interact(entt::registry& registry, entt::entity bush){
            auto[cHasBerries, cTexture] = registry.get<CHasBerrys, CTexture>(bush);
            cTexture.texture.src.rect = ecs::textures::bush_no_berries;
            cHasBerries.time_collected = std::chrono::high_resolution_clock::now();
            cHasBerries.value = false;
        }
        static void tick(entt::registry& registry, entt::entity bush, engine_time::Time time){
            auto[c_has_berries, c_texture, c_source] = registry.get<CHasBerrys, CTexture, CSource>(bush);
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            float time_since_berries_collected_ms = std::chrono::duration<float, std::milli>(now - c_has_berries.time_collected).count();
            float& tile_life_matter = registry.get<CComposition>(c_source.source).life_matter;
            if (tile_life_matter > c_source.life_matter_consume_per_ms * time.delta_time_ms)
                tile_life_matter -= c_source.life_matter_consume_per_ms * time.delta_time_ms;
            else{
                registry.destroy(bush);
                return;
            }
            if (time_since_berries_collected_ms > 3000){
                c_has_berries.value = true;
                c_texture.texture.src.rect = ecs::textures::bush_with_berries;
            }
        }
        inline entt::entity newBush(glm::vec2 pos, Texture2DBundle texture,entt::registry& registry, entt::entity source){
            entt::entity bush = registry.create();
            registry.emplace<ecs::CPosition>(bush, pos);
            registry.emplace<ecs::CTexture>(bush, texture);
            registry.emplace<CHasBerrys>(bush, true);
            registry.emplace<ecs::CTick>(bush, tick);
            registry.emplace<ecs::CSource>(bush, source, 0.03f);
            registry.emplace<ecs::CInteractable>(bush,"Collect Berries", ecs::CInteractable::InteractionType::Gader, can_interact, interact);
            return bush;
        }
        inline Texture2DBundle defaultTextureBundle(){
            return Texture2DBundle{
                .src = TextureSrc{
                    .key = TextureKeys::Bush,
                        .rect = textures::bush_with_berries
                },
                    .size = {(float)config::render_tile_size, (float)config::render_tile_size},
                   /// .color = RAYWHITE,
                    ///.rotation = 0
            };
        }
    };
}
