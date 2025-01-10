#pragma once
#include <random>
#include <cmath>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Syris/scripts/DynamicLibraryLoader.hpp"

#include "Components.h"
#include "Sandbox/scene/helper.h"
#include "Sandbox/texture/SandboxTexture.hpp"
namespace Sandbox::ecs {
    namespace Chicken{
        void chick() {};
        /*
        inline void tick(Syris::EntityManager& manager, entt::entity chicken, const Syris::engine_time::Time& time){
            auto [pos, move_speed, current_speed, dir]= manager.get_registry().get<Syris::ecs::ChunkedPosition, CMovementSpeed, CSpeed, CDir>(chicken);
            pos.pos += time.get_delta_ms() * (current_speed.speed / 1000.f ) * dir.value;
    
            QuadTexInstancedData instance_data = QuadTexInstancedData();
            instance_data.tex_coord = { texture::atlas::chicken.min, texture::atlas::chicken.max };
            instance_data.translation = glm::translate(instance_data.translation, glm::vec3(pos.pos, 1.f));
            
            // BREAK_POINT("TODO!");
            std::array<std::pair<size_t, void *>, 1> entity_data;
            entity_data[0] = {0, &instance_data};
            Syris::BR_RequestSparse request{
                .entity = chicken,
                .data = {entity_data.begin(), entity_data.end()} //&instance_data
            };
            manager.get_batch_renderer_manager().set_entity(manager.get_registry().ctx().get<SER_ID>().renderer_id, request);
        }*/
        /*
        inline entt::entity newChickenEntity(glm::vec2 pos,
            Syris::EntityManager& entity_manager,
            Syris::BatchRendererManager::BR_ID material_id,
            Syris::DynamicLibraryLoader& dll_loader) {

            QuadTexInstancedData instance_data = QuadTexInstancedData();
            instance_data.tex_coord = { texture::atlas::chicken.min, texture::atlas::chicken.max };
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
            auto res = dll_loader.get_function_ptr<void(Syris::EntityManager&, entt::entity, const Syris::engine_time::Time& time)>("chicken_tick");
            if (!res)
                BREAK_POINT("Cant find function");
            
            //Syris::FastFunction<void(entt::entity, Syris::EntityManager&, const Syris::engine_time::Time& time)> fast_tick(res.value());
            return entity_manager.new_entity(info, [&entity_manager, pos, res](entt::entity player){
                entt::registry& registry = entity_manager.get_registry();
                registry.emplace<Syris::ecs::ChunkedPosition>(player, pos);
                registry.emplace<CSpeed>(player, glm::vec2(0.1f));
                registry.emplace<CMovementSpeed>(player, 3.f);
                registry.emplace<CDir>(player, glm::vec2(0.f, 1.f));
                registry.emplace<CTickFast>(player, res.value());
            });
        }
        */
        /*inline entt::entity new_instance(glm::vec2 pos,
            Syris::EntityManager& entity_manager,
            Syris::BatchRendererManager::BR_ID material_id,
            Syris::DynamicLibraryLoader& dll_loader) {

            QuadTexInstancedData instance_data = QuadTexInstancedData();
            instance_data.tex_coord = { texture::atlas::chicken.min, texture::atlas::chicken.max };
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
            auto res = dll_loader.get_function_ptr<void(Syris::EntityManager&, entt::entity, const Syris::engine_time::Time& time)>("chicken_tick");
            if (!res)
                BREAK_POINT("Cant find function");
            
            //Syris::FastFunction<void(entt::entity, Syris::EntityManager&, const Syris::engine_time::Time& time)> fast_tick(res.value());
            return entity_manager.new_entity(info, [&entity_manager, pos, res](entt::entity player){
                entt::registry& registry = entity_manager.get_registry();
                registry.emplace<Syris::ecs::ChunkedPosition>(player, pos);
                registry.emplace<CSpeed>(player, glm::vec2(0.1f));
                registry.emplace<CMovementSpeed>(player, 3.f);
                registry.emplace<CDir>(player, glm::vec2(0.f, 1.f));
                registry.emplace<CTickFast>(player, res.value());
            });
        }*/

        inline std::tuple<QuadTexInstancedData> on_create(Syris::EntityManager& entity_manager, entt::entity chicken, glm::vec2&& pos){
            //this shuld be backed in at least part of it
            static QuadTexInstancedData instance_data{
                {texture::atlas::chicken.min, texture::atlas::chicken.max},
                glm::mat4(1.f)
            };
            //how do we get pos??
            instance_data.translation = glm::translate(instance_data.translation, glm::vec3(pos, 1.f));

            entt::registry &registry = entity_manager.get_registry();
            registry.emplace<Syris::ecs::ChunkedPosition>(chicken, pos);
            registry.emplace<CSpeed>(chicken, glm::vec2(0.1f));
            registry.emplace<CMovementSpeed>(chicken, 3.f);
            registry.emplace<CDir>(chicken, glm::vec2(0.f, 1.f));
            //registry.emplace<CTickFast>(chicken, res.value());
            return {instance_data};
        }
    }
}
