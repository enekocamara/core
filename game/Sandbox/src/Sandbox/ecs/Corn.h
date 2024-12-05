#pragma  once
#include <atomic>
#include <glm/glm.hpp>
#include "Syris/Libs.h"
#include "Syris/ecs/EntityManager.hpp"

#include "Components.h"
#include "Collectable.h"
#include "../texture/SandboxTexture.hpp"
#include <entt.hpp>
#include "Sandbox/scene/helper.h"

namespace Sandbox::ecs::Corn {

    inline CCollectable get_collectable_info(entt::registry& registry){
        static bool first = true;
        static CollectableManager::ID id;
        if (first){
            std::vector<Stages> stages;
            stages.push_back(Stages{
                .texture = {texture::atlas::corn_stage_0.min, texture::atlas::corn_stage_0.max},
                .threashold = 0.f,
                .comsuption_ms = 0.01f,
            });
            stages.push_back(Stages{
                .texture = {texture::atlas::corn_stage_1.min, texture::atlas::corn_stage_1.max},
                .threashold = 10.f,
                .comsuption_ms = 0.01f,
            });
            stages.push_back(Stages{
                .texture = {texture::atlas::corn_stage_2.min, texture::atlas::corn_stage_2.max},
                .threashold = 20.f,
                .comsuption_ms = 0.01f,
            });
            stages.push_back(Stages{
                .texture = {texture::atlas::corn_stage_3.min, texture::atlas::corn_stage_3.max},
                .threashold = 30.f,
                .comsuption_ms = 0.02f,
            });
            id = registry.ctx().get<CollectableManager>().set_collectable(std::move(stages));
            first = false;
        }
        return CCollectable{
            .id = id,
            .current_life_matter = 0,
            .current_stage = 0
        };
    }
    
    inline entt::entity new_corn_entity(glm::vec2 pos, Syris::EntityManager &entity_manager, entt::entity source, const Syris::engine_time::Time& time){
        return Collectable::new_collectable_entity(pos, get_collectable_info(entity_manager.get_registry()), entity_manager, source, time);
    }
};
