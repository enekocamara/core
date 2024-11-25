#pragma  once
#include <atomic>
#include <glm/glm.hpp>
#include "Syris/Libs.h"
#include "Syris/ecs/EntityManager.hpp"

#include "Components.h"
#include "../texture/SandboxTexture.hpp"
#include <entt.hpp>
#include "Sandbox/scene/helper.h"
namespace Sandbox::ecs::Collectable {
    
    static void tick(Syris::EntityManager& entity_manager, entt::entity collectable, const Syris::engine_time::Time& time){
        entt::registry& registry = entity_manager.get_registry();
        auto[c_collectable, c_source] = registry.get<CCollectable, CSource>(collectable);
        auto stages = registry.ctx().get<CollectableManager>().get_collectable(c_collectable.id);
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        float& tile_life_matter = entity_manager.get_registry().get<CComposition>(c_source.source).life_matter;
        float comsuption = stages[c_collectable.current_stage].comsuption_ms * time.get_delta_ms();
        if (tile_life_matter > comsuption){
            tile_life_matter -= comsuption; 
            c_collectable.current_life_matter += comsuption;
            if (c_collectable.current_stage < stages.size() - 1){
                if (c_collectable.current_life_matter > stages[c_collectable.current_stage + 1].threashold){
                    c_collectable.current_stage++;
                    QuadTexInstancedData instance_data{
                        .tex_coord = {stages[c_collectable.current_stage].texture.min, stages[c_collectable.current_stage].texture.max},
                        .translation = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(entity_manager.get_registry().get<CPosition>(collectable).pos, 1.f)), {0.5, 0.5, 1.f}),
                    };
                    Syris::BR_SetRequest request{
                        .entity = collectable,
                        .data = &instance_data
                    };
                    entity_manager.get_batch_renderer_manager().set_entity(entity_manager.get_registry().ctx().get<SER_ID>().renderer_id, request);
                }
            }
        } else{
            Syris::BR_RemoveRequest request = {collectable};
            entity_manager.get_batch_renderer_manager().get_renderer(entity_manager.get_registry().ctx().get<SER_ID>().renderer_id)->remove_entity(request);
            entity_manager.delete_entity(collectable);
        }
    }

    inline entt::entity new_collectable_entity(glm::vec2 pos, CCollectable collectable_info, Syris::EntityManager &entity_manager, entt::entity source, const Syris::engine_time::Time& time)
    {
        QuadTexInstancedData instance_data;
        auto stages = entity_manager.get_registry().ctx().get<CollectableManager>().get_collectable(collectable_info.id);
        instance_data.tex_coord = {stages[collectable_info.current_stage].texture.min, stages[collectable_info.current_stage].texture.max},
        instance_data.translation = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(pos, 1.f)), {0.5,0.5,1.f});

        // entity system part
        Syris::EntityManager::RenderInfo render_info{
            .renderer = entity_manager.get_registry().ctx().get<SER_ID>().renderer_id,
            //.size = sizeof(TileInstancedData),
            .entity_data = &instance_data
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info
        };
        entt::registry &registry = entity_manager.get_registry();
        entt::entity collectable = entity_manager.new_entity(info);
        registry.emplace<CPosition>(collectable, pos);
        registry.emplace<CCollectable>(collectable, collectable_info);
        registry.emplace<CTickFast>(collectable, tick);
        registry.emplace<CSource>(collectable, source, 0.01f);
        //registry.emplace<CInteractable>(collectable, "Collect Berries", ecs::CInteractable::InteractionType::Gader,  can_interact ,interact);
        return collectable;
    }
};
