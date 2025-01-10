#include <entt.hpp>
#include <cstdlib>
#include <ctime>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/Libs.h"
#include "Syris/log/Log.h"

#include "Sandbox/world_generator/TileMaterial.hpp"
#include "Sandbox/ecs/Components.h"
#include "Sandbox/ecs/Bush.h"
#include "Sandbox/ecs/Corn.h"
#include "Syris/scripts/Export.h"
#include "Sandbox/ecs/Tile.h"

namespace Sandbox::ecs::Tile {

    EXPORT_FUNCTION void tile_on_update(Syris::EntityManager& entity_manager, entt::entity entity, const Syris::engine_time::Time& time){
        auto[c_composition, c_position, c_data] =  entity_manager.get_registry().get<CComposition, CPosition, AsyncComponent<CTileData>>(entity);
        c_composition.life_matter += time.get_delta_ms() * c_composition.regen_per_ms;
        if (c_composition.life_matter > 80){
            c_composition.life_matter -= 40;
            auto data = c_data.get();
            //if (data.heat > 0.2f && data.humidity > 0.2f && data.height < 0.3 && data.height > 0.f)
            auto renderer_id = entity_manager.get_registry().ctx().get<entities::SER_ID>().renderer_id;
            //std::cout << "generating new collectable\n";
            //return;
            if (/*data.heat > 0.2f && data.humidity > 0.2f && data.height < 0.3 &&*/ data.height > 0.0f){
                entity_manager.new_entity_dll_better<std::tuple<QuadTexInstancedData>, glm::vec2, CCollectable, entt::entity, const Syris::engine_time::Time&>
                    (renderer_id, "collectable", c_position.pos, ecs::Corn::get_collectable_info(entity_manager.get_registry()), entity, time);
            }
            else if(data.humidity > 0.f && data.heat > -0.8 && data.height > 0.5f) {
                entity_manager.new_entity_dll_better<std::tuple<QuadTexInstancedData>, glm::vec2, CCollectable, entt::entity, const Syris::engine_time::Time&>
                    (renderer_id, "collectable", c_position.pos, ecs::Bush::get_collectable_info(entity_manager.get_registry()), entity, time);
            }
        }
    }

    EXPORT_FUNCTION void tile_on_create(Syris::EntityManager& entity_manager, entt::entity entity, Info info){
        auto &registry = entity_manager.get_registry();

        registry.emplace<CTile>(entity, info.index);
        registry.emplace<CPosition>(entity, info.pos);
        //registry.emplace<CTickFast>(entity, tick);
        registry.emplace<CComposition>(entity, info.init_life_matter, 0.01f);
        registry.emplace<AsyncComponent<CTileData>>(entity, info.data);
    }
    /*
    inline Syris::Texture2DBundle defaultTextureBundle(){
        return Syris::Texture2DBundle{
            .src = texture::atlas::grass_0,
            .size = {(float)Syris::config::render_tile_size, (float)Syris::config::render_tile_size},
                ///       .color = RAYWHITE,
                ///       .rotation = 0
        };
    }*/
}
