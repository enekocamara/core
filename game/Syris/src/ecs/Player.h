#pragma once
#include "../Libs.h"
#include "Components.h"
#include "../EngineTime.h"
#include <cmath>

namespace ecs {
    namespace Player{
        enum class AnimationDir{
            Up,
            Down,
            Left,
            Right
        };
        inline void animate(entt::registry& registry, entt::entity entity, engine_time::Time time, Texture2DBundle& texture){
            auto[dir,speed] = registry.get<CDir, CSpeed>(entity);
            bool iddle = speed.value == 0.f;
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            int index;
            if (iddle)
                index = (int)(std::chrono::duration<float, std::milli>(now - time.init_time).count() / 1000) % 2 == 0;
            else
                index = (int)(std::chrono::duration<float, std::milli>(now - time.init_time).count() / 250) % 2 == 0;
            ecs::textures::Player::Dir animation_dir;

            if (std::abs(dir.value.x) > std::abs(dir.value.y))
                animation_dir = (dir.value.x >= 0) ? ecs::textures::Player::Dir::Right : ecs::textures::Player::Dir::Left;
            else{
                animation_dir = (dir.value.y > 0) ? ecs::textures::Player::Dir::Down : ecs::textures::Player::Dir::Up;
            }
            ///texture.src.rect = ecs::textures::Player::getPlayerTextureRectangle(animation_dir, iddle,index);
        }
        inline entt::entity newPlayer(glm::vec2 pos, MovementKeys keys, Texture2DBundle textureBundle, entt::registry& registry){
            entt::entity entity = registry.create();
            registry.emplace<CPosition>(entity, pos);
            registry.emplace<CTexture>(entity, textureBundle);
            registry.emplace<CKeyBinded>(entity, keys);
            registry.emplace<CSpeed>(entity, 0.f);
            registry.emplace<CDir>(entity, glm::vec2(0.f,-1.f));
            registry.emplace<CAnimated>(entity, animate);
            return entity;
        }
        inline Texture2DBundle defaultTextureBundle(){
            return Texture2DBundle{
                .src = TextureSrc{
                    .key = TextureKeys::Player,
                        .rect = ecs::textures::Player::getPlayerTextureRectangle(ecs::textures::Player::Dir::Down, true, 0)
                },
                    .size = {config::render_tile_size, config::render_tile_size},
            ///        .color = RAYWHITE,
            ///        .rotation = 0
            };
        }
        
    }
}
