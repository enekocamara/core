#pragma  once
#include <atomic>
#include <glm/glm.hpp>
#include "Syris/Libs.h"
#include "Syris/ecs/EntityManager.hpp"

#include "Components.h"
#include "../texture/SandboxTexture.hpp"
#include <entt.hpp>
#include "Sandbox/scene/helper.h"

namespace Sandbox::ecs::Corn {
    /*
    struct CHasCorn{

        bool value;
        std::chrono::high_resolution_clock::time_point time_collected;
    };
    static bool can_interact(entt::registry& registry, entt::entity bush){
        return  registry.get<CHasCorn>(bush).value;
    }
    static void interact(entt::registry& registry, entt::entity bush){
        auto [cHasBerries, cTexture] = registry.get<CHasBerrys, CTexture>(bush);
        cTexture.rect = texture::atlas::bush_no_berries;
        cHasBerries.time_collected = std::chrono::high_resolution_clock::now();
        cHasBerries.value = false;
    }
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
            entity_manager.get_materials().get_material(entity_manager.get_registry().ctx().get<SMaterialID>().material_id)->remove_entity(request);
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
            entity_manager.get_materials().set_entity(entity_manager.get_registry().ctx().get<SMaterialID>().material_id, request);
        }
    }
    inline entt::entity newBushEntity(glm::vec2 pos, Syris::texture::Texture2DBundle textureBundle, Syris::EntityManager &entity_manager, entt::entity source, const Syris::engine_time::Time& time)

    {
        // rendering part
        //TileVertices tile_vertices = TileVertices();
        TileInstancedData *instance_data = new TileInstancedData();
        instance_data->tex_coord = {texture::atlas::bush_no_berries.min, texture::atlas::bush_no_berries.max};
        instance_data->translation = glm::scale(glm::translate(instance_data->translation, glm::vec3(pos, 1.f)), {0.5,0.5,1.f});
        // entity system part
        Syris::EntityManager::RenderInfo render_info{
            .material = entity_manager.get_registry().ctx().get<SMaterialID>().material_id,
            //.size = sizeof(TileInstancedData),
            .entity_data = instance_data
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info
        };
        entt::registry &registry = entity_manager.get_registry();
        entt::entity bush = entity_manager.new_entity(info);
        registry.emplace<CPosition>(bush, pos);
        registry.emplace<CTexture>(bush, textureBundle.src);
        registry.emplace<CHasBerrys>(bush, false, time.get_frame_start());
        registry.emplace<CTickFast>(bush, tick);
        registry.emplace<CSource>(bush, source, 0.01f);
        registry.emplace<CInteractable>(bush, "Collect Berries", ecs::CInteractable::InteractionType::Gader,  can_interact ,interact);
        return bush;
    }
    inline Syris::texture::Texture2DBundle defaultTextureBundle()
    {
        return Syris::texture::Texture2DBundle{
            .src = texture::atlas::bush_with_berries,
            .size = {
                (float)Syris::config::render_tile_size,
                (float)Syris::config::render_tile_size
            },
                /// .color = RAYWHITE,
                ///.rotation = 0
        };
    }
    */
};
