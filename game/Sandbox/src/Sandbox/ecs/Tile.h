#pragma once

#include <entt.hpp>
#include <cstdlib>
#include <ctime>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/Libs.h"
#include "Syris/log/Log.h"

#include "Sandbox/world_generator/TileMaterial.hpp"
#include "Components.h"
#include "Bush.h"
#include "Corn.h"
namespace Sandbox::ecs::Tile {
    //this needs to be reworked asap
    //static Syris::BatchRendererManager::ER_ID g_tile_material_id = 0;
    //static Syris::BatchRendererManager::ER_ID g_entity_material_id = 0;
    struct SER_ID{
        Syris::BatchRendererManager::BR_ID renderer_id;
    };

    static void tick(Syris::EntityManager& entity_manager, entt::entity entity, const Syris::engine_time::Time& time){
        auto[c_composition, c_position, c_data] =  entity_manager.get_registry().get<CComposition, CPosition, CTileData>(entity);
        c_composition.life_matter += time.get_delta_ms() * c_composition.regen_per_ms;
        if (c_composition.life_matter > 80){
            c_composition.life_matter -= 40;
            return;
            if (c_data.heat > 0.5f && c_data.humidity > 0.2f && c_data.height < 0.7f)
                ecs::Corn::new_corn_entity(c_position.pos, entity_manager, entity, time);
            else if(c_data.humidity > 0.f && c_data.heat > -0.8 && c_data.height > 0.2f)
                ecs::Bush::new_bush_entity(c_position.pos,  entity_manager, entity, time);
        }
    }

    inline entt::entity newTile(TileInstancedData* data, Syris::EntityManager& entity_manager, uint32_t index, float init_life_matter, glm::vec2 pos){
        // m_material->set_data(data_set);
        Syris::EntityManager::RenderInfo render_info{
            .renderer = entity_manager.get_registry().ctx().get<SER_ID>().renderer_id,
            .entity_data = data,
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info};
        entt::entity entity = entity_manager.new_entity(info);
        entt::registry &registry = entity_manager.get_registry();
        registry.emplace<CTile>(entity);
        registry.emplace<CPosition>(entity, pos);
        registry.emplace<CTickFast>(entity, tick);
        registry.emplace<CComposition>(entity, init_life_matter, 0.01f);
        registry.emplace<CTileData>(entity, *(CTileData*)&data->mat);
        return entity;
    }
    
    inline Syris::Texture2DBundle defaultTextureBundle(){
        return Syris::Texture2DBundle{
            .src = texture::atlas::grass_0,
            .size = {(float)Syris::config::render_tile_size, (float)Syris::config::render_tile_size},
                ///       .color = RAYWHITE,
                ///       .rotation = 0
        };
    }
}
