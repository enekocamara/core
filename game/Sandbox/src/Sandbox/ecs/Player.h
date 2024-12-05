#pragma once

#include "Syris/Libs.h"
#include "Components.h"
#include "../texture/SandboxTexture.hpp"
#include <cmath>
#include <tuple>
#include <glm/glm.hpp>

namespace Sandbox::ecs {
    namespace Player{
        enum class AnimationDir{
            Up,
            Down,
            Left,
            Right
        };

        //the components that make up the player entity
        using ComponentsTuple = std::tuple<AsyncComponent<CPosition>,CTexture,CKeyBinded,CSpeed,CDir>;

        inline void animate(entt::registry& registry, entt::entity entity, Syris::engine_time::Time time){
            
            auto[dir,speed, texture] = registry.get<CDir, CSpeed, CTexture>(entity);
            bool iddle = speed.speed.x == 0 && speed.speed.y == 0;
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            int index;
            if (iddle)
                index = (int)(time.get_running_time_ms() / 1000) % 2 == 0;
            else
                index = (int)(time.get_running_time_ms() / 250) % 2 == 0;
            texture::Player::Dir animation_dir;

            if (std::abs(dir.value.x) > std::abs(dir.value.y))
                animation_dir = (dir.value.x >= 0) ? texture::Player::Dir::Right : texture::Player::Dir::Left;
            else{
                animation_dir = (dir.value.y > 0) ? texture::Player::Dir::Down : texture::Player::Dir::Up;
            }
            texture.rect = texture::Player::getPlayerTextureRectangle(animation_dir, iddle, index);
            ///texture.src.rect = ecs::textures::Player::getPlayerTextureRectangle(animation_dir, iddle,index);
        }
        /*inline entt::entity newPlayer(glm::vec2 pos, MovementKeys keys, Syris::texture::Texture2DBundle textureBundle, entt::registry& registry){
            entt::entity entity = registry.create();
            registry.emplace<AsyncComponent<CPosition>>(entity, pos);
            registry.emplace<CTexture>(entity, textureBundle.src);
            registry.emplace<CKeyBinded>(entity, keys);
            registry.emplace<CSpeed>(entity, 0.f);
            registry.emplace<CDir>(entity, glm::vec2(0.f, -1.f));
            //registry.emplace<CAnimated>(entity, animate);
            return entity;
        }*/
        inline entt::entity newPlayerEntity(glm::vec2 pos,
            MovementKeys keys,
            Syris::Texture2DBundle textureBundle,
            Syris::EntityManager& entity_manager,
            Syris::BatchRendererManager::BR_ID material_id) {


            QuadTexInstancedData instance_data = QuadTexInstancedData();
            instance_data.tex_coord = { texture::atlas::player_0.min, texture::atlas::player_0.max };
            instance_data.translation = glm::translate(instance_data.translation, glm::vec3(pos, 1.f));

            //entity system part
            std::array<std::pair<std::size_t, void*>, 1> entity_data;
            entity_data[0] = { 0, &instance_data };
            Syris::EntityManager::RenderInfo render_info{
                .renderer = material_id,
                .request = {entity_data.begin(), entity_data.end()}
            };
            Syris::EntityManager::EntityInfo info{
                .render_info = render_info
            };
            entt::registry& registry = entity_manager.get_registry();
            return entity_manager.new_entity(info, [&entity_manager, pos, textureBundle, keys](entt::entity player) {
                auto& registry = entity_manager.get_registry();
                registry.emplace<AsyncComponent<CPosition>>(player, pos);
                registry.emplace<CTexture>(player, textureBundle.src);
                registry.emplace<CKeyBinded>(player, keys);
                registry.emplace<CSpeed>(player, glm::vec2(0.f));
                registry.emplace<CMovementSpeed>(player, 3.f);
                registry.emplace<CDir>(player, glm::vec2(0.f, -1.f));
            });

        }
        inline Syris::Texture2DBundle defaultTextureBundle(){
            return Syris::Texture2DBundle{
                .src = texture::Player::getPlayerTextureRectangle(texture::Player::Dir::Down, true, 0),
                .size = {config::render_tile_size, config::render_tile_size},
                    ///        .color = RAYWHITE,
                    ///        .rotation = 0
            };
        }
        inline void sync(Syris::EntityManager& entity_manager,
                         Syris::BatchRendererManager& material_manager,
                         Syris::BatchRendererManager::BR_ID material_id,
                         entt::entity player){
            QuadTexInstancedData player_data = QuadTexInstancedData();
            auto cPos = entity_manager.get_registry().get<ecs::AsyncComponent<ecs::CPosition>>(player);
            player_data.tex_coord = {texture::atlas::player_0.min, texture::atlas::player_0.max};
            player_data.translation = glm::translate(player_data.translation, glm::vec3(cPos.get().pos, 1.f));
            //BREAK_POINT("TODO!");
            std::array<std::pair<std::size_t, void*>, 1> entity_data;
            entity_data[0] = { 0, &player_data };
            Syris::BR_RequestSparse request{
                .entity = player,
                .data = {entity_data.begin(), entity_data.end()},//&player_data
            };
            material_manager.set_entity(material_id, request);
        }
        
    }
}
