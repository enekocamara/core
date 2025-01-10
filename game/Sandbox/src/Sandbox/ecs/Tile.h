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
    /*
    inline void tick(Syris::EntityManager& entity_manager, entt::entity entity, const Syris::engine_time::Time& time){
        auto[c_composition, c_position, c_data] =  entity_manager.get_registry().get<CComposition, CPosition, AsyncComponent<CTileData>>(entity);
        c_composition.life_matter += time.get_delta_ms() * c_composition.regen_per_ms;
        if (c_composition.life_matter > 80){
            c_composition.life_matter -= 40;
            auto data = c_data.get();
            //if (data.heat > 0.2f && data.humidity > 0.2f && data.height < 0.3 && data.height > 0.f)
            auto renderer_id = entity_manager.get_registry().ctx().get<SER_ID>().renderer_id;
            if (data.height > 0.f)
                entity_manager.new_entity_dll_better<std::tuple<QuadTexInstancedData>, glm::vec2, CCollectable, entt::entity, const Syris::engine_time::Time&>
                    (renderer_id, "collectable", c_position.pos, ecs::Corn::get_collectable_info(entity_manager.get_registry()), entity, time);
                //ecs::Corn::new_corn_entity(c_position.pos, entity_manager, entity, time);
            else if(data.humidity > 0.f && data.heat > -0.8 && data.height > 0.5f)
                entity_manager.new_entity_dll_better<std::tuple<QuadTexInstancedData>, glm::vec2, CCollectable, entt::entity, const Syris::engine_time::Time&>
                    (renderer_id, "collectable", c_position.pos, ecs::Bush::get_collectable_info(entity_manager.get_registry()), entity, time);
                //ecs::Bush::new_bush_entity(c_position.pos,  entity_manager, entity, time);
        }
    }*/
    struct Info {
        std::size_t index;
        float init_life_matter;
        glm::vec2 pos;
        CTileData data;
    };
/*
    inline void on_create(Syris::EntityManager& entity_manager, entt::entity entity, Info info){
        auto &registry = entity_manager.get_registry();

        registry.emplace<CTile>(entity, info.index);
        registry.emplace<CPosition>(entity, info.pos);
        //registry.emplace<CTickFast>(entity, tick);
        registry.emplace<CComposition>(entity, info.init_life_matter, 0.01f);
        registry.emplace<AsyncComponent<CTileData>>(entity, info.data);
    }
    */
    inline Syris::Texture2DBundle defaultTextureBundle(){
        return Syris::Texture2DBundle{
            .src = texture::atlas::grass_0,
            .size = {(float)Syris::config::render_tile_size, (float)Syris::config::render_tile_size},
                ///       .color = RAYWHITE,
                ///       .rotation = 0
        };
    }
}
