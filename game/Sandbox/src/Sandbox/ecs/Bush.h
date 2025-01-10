#pragma  once
#include <atomic>
#include <glm/glm.hpp>

#include <entt.hpp>

#include "Syris/Libs.h"
#include "Syris/ecs/EntityManager.hpp"

#include "Sandbox/scene/helper.h"
#include "Sandbox/texture/SandboxTexture.hpp"

#include "Components.h"
#include "Collectable.h"

namespace Sandbox::ecs::Bush {
    inline std::atomic<uint64_t> g_number_of_bushes;

    struct CHasBerrys{
        bool value;
        std::chrono::high_resolution_clock::time_point time_collected;
    };
    static bool can_interact(entt::registry& registry, entt::entity bush){
        return  registry.get<CHasBerrys>(bush).value;
    }
    static void interact(entt::registry& registry, entt::entity bush){
        auto [cHasBerries, cTexture] = registry.get<CHasBerrys, CTexture>(bush);
        cTexture.rect = texture::atlas::bush_no_berries;
        cHasBerries.time_collected = std::chrono::high_resolution_clock::now();
        cHasBerries.value = false;
    }
    /*
    static void tick(Syris::EntityManager& entity_manager, entt::entity bush, const Syris::engine_time::Time& time){

        auto[c_has_berries, c_texture, c_source] = entity_manager.get_registry().get<CHasBerrys, CTexture, CSource>(bush);
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        float time_since_berries_collected_ms = std::chrono::duration<float, std::milli>(now - c_has_berries.time_collected).count();
        float& tile_life_matter = entity_manager.get_registry().get<CComposition>(c_source.source).life_matter;
        float comsuption;
        if (c_has_berries.value == true)
            comsuption = c_source.life_matter_consume_per_ms * time.get_delta_ms() * 1.5;
        else
            comsuption = c_source.life_matter_consume_per_ms * time.get_delta_ms();
        if (tile_life_matter > comsuption)
            tile_life_matter -= comsuption;
        else{
            Syris::MaterialRemoveRequest request = {bush};
            entity_manager.get_materials().get_material(entity_manager.get_registry().ctx().get<SER_ID>().material_id)->remove_entity(request);
            entity_manager.delete_entity(bush);
            return;
        }
        if (time_since_berries_collected_ms > 3000){
            c_has_berries.value = true;

            //
            TileInstancedData instance_data{
                .tex_coord = {texture::atlas::bush_with_berries.min, texture::atlas::bush_with_berries.max}, 
                .translation = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(entity_manager.get_registry().get<CPosition>(bush).pos , 1.f)), {0.5, 0.5, 1.f}),
            };
            Syris::MaterialSetRequest request{
                .entity = bush,
                .data = &instance_data
            };
            entity_manager.get_materials().set_entity(entity_manager.get_registry().ctx().get<SER_ID>().material_id, request);
        }
    }
*/
    inline CCollectable get_collectable_info(entt::registry& registry){
        static bool first = true;
        static CollectableManager::ID id;
        if (first){
            std::vector<Stages> stages;
            stages.push_back(Stages{
                .texture = {texture::atlas::bush_no_berries.min, texture::atlas::bush_no_berries.max},
                .threashold = 0.f,
                .comsuption_ms = 0.01f,
            });
            stages.push_back(Stages{
                .texture = {texture::atlas::bush_with_berries.min, texture::atlas::bush_with_berries.max},
                .threashold = 20.f,
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
    /*
    inline entt::entity new_bush_entity(glm::vec2 pos, Syris::EntityManager &entity_manager, entt::entity source, const Syris::engine_time::Time& time)
    {
        return Collectable::new_collectable_entity(pos, get_collectable_info(entity_manager.get_registry()), entity_manager, source, time);
    }
    */
    inline Syris::Texture2DBundle defaultTextureBundle()
    {
        return Syris::Texture2DBundle{
            .src = texture::atlas::bush_with_berries,
            .size = {
                (float)Syris::config::render_tile_size,
                (float)Syris::config::render_tile_size
            },
                /// .color = RAYWHITE,
                ///.rotation = 0
        };
    }
};
