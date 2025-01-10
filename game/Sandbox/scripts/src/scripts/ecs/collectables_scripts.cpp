#include <atomic>
#include <glm/glm.hpp>
#include <entt.hpp>

#include "Syris/Libs.h"
#include "Syris/ecs/EntityManager.hpp"
#include "Sandbox/ecs/Components.h"
#include "Sandbox/texture/SandboxTexture.hpp"
#include "Sandbox/scene/helper.h"
#include "Syris/scripts/Export.h"
namespace Sandbox::ecs::Collectable {
    EXPORT_FUNCTION void collectable_on_update(Syris::EntityManager& entity_manager, entt::entity collectable, const Syris::engine_time::Time& time){
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
                        .translation = glm::translate(glm::mat4(1.f), glm::vec3(entity_manager.get_registry().get<Syris::ecs::ChunkedPosition>(collectable).pos, 1.f)),
                    };
                    //BREAK_POINT("TODO!");
                    std::array<std::pair<size_t, void*>, 1> entity_data;
                    entity_data[0] = { 0, &instance_data };
                    Syris::BR_RequestSparse request{
                        .entity = collectable,
                        .data = {entity_data.begin(), entity_data.end() }//&instance_data
                    };
                    entity_manager.get_batch_renderer_manager().set_entity(entity_manager.get_registry().ctx().get<entities::SER_ID>().renderer_id, request);
                }
            }
        } else{
            Syris::BR_RemoveRequest request = {collectable};
            entity_manager.get_batch_renderer_manager().get_renderer(entity_manager.get_registry().ctx().get<entities::SER_ID>().renderer_id)->remove_entity(request);
            entity_manager.delete_entity(collectable);
        }
    }

    EXPORT_FUNCTION void collectable_on_create(Syris::EntityManager& entity_manager, entt::entity collectable,std::tuple<QuadTexInstancedData>& instance_data, glm::vec2 pos, CCollectable collectable_info, entt::entity source, const Syris::engine_time::Time& time)
    {
        auto stages = entity_manager.get_registry().ctx().get<CollectableManager>().get_collectable(collectable_info.id);
        
        std::get<0>(instance_data).tex_coord = {stages[collectable_info.current_stage].texture.min, stages[collectable_info.current_stage].texture.max};
        std::get<0>(instance_data).translation = glm::translate(glm::mat4(1.f), glm::vec3(pos, 1.f));

        entt::registry &registry = entity_manager.get_registry();
        registry.emplace<Syris::ecs::ChunkedPosition>(collectable, pos);
        registry.emplace<CCollectable>(collectable, collectable_info);
        registry.emplace<CSource>(collectable, source, 0.01f);
        //registry.emplace<CInteractable>(collectable, "Collect Berries", ecs::CInteractable::InteractionType::Gader,  can_interact ,interact);

    }
};