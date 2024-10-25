#pragma once

#include <entt.hpp>
#include <cstdlib>
#include <ctime>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/Libs.h"
#include "Syris/log/Log.h"

#include "Components.h"
#include "Bush.h"
namespace Sandbox::ecs::Tile {
    //this needs to be reworked asap
    //static Syris::MaterialManager::MaterialID g_tile_material_id = 0;
    //static Syris::MaterialManager::MaterialID g_entity_material_id = 0;
    struct SMaterialID{
        Syris::MaterialManager::MaterialID material_id;
    };

    static void tick(Syris::EntityManager& entity_manager, entt::entity entity, const Syris::engine_time::Time& time){
        //return;
        auto[c_composition, c_position, c_tile] =  entity_manager.get_registry().get<CComposition, CPosition, CTile>(entity);
        /*c_tile.mutex.lock();
        if (c_tile.in_use){
            std::cout << "multiple threads accesing the same entity at the same time. Entity: " << (uint64_t)entity << std::endl; 
            exit(1);
        }
        c_tile.in_use = true;
        c_tile.mutex.unlock();
        */
        c_composition.life_matter += time.get_delta_ms() * c_composition.regen_per_ms;
        if (c_composition.life_matter > 80 &&  c_tile.type == ecs::CTile::TileType::Grass){
            c_composition.life_matter -= 40;
        //  c_tile.in_use = false;
        //    return;
            int choice = rand() % 3;
            if (choice == 0)
                ecs::Bush::newBushEntity(c_position.pos,  entity_manager, entity, time);
            else if (choice == 1)
                ecs::Bush::newBushEntity(c_position.pos, entity_manager, entity, time);
            else if (choice == 2)
                ecs::Bush::newBushEntity(c_position.pos, entity_manager, entity, time);
            else
                exit(1);
        }
        //c_tile.in_use = false;
    }

    inline entt::entity newTile(glm::vec2 pos, Syris::texture::Rectangle2D texture, Syris::EntityManager& entity_manager, ecs::CTile::TileType type,  uint32_t index){
            
            //set model matrix
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(pos, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
            
            //data to change
                //m_buffer->bind(1);
            TileInstancedData data;
            data.translation = model;
            data.tex_coord = {texture.min, texture.max};
           
            //m_material->set_data(data_set);
            Syris::EntityManager::RenderInfo render_info{
                .material = entity_manager.get_registry().ctx().get<SMaterialID>().material_id,
                .entity_data = &data,
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info 
        };
        entt::entity entity = entity_manager.new_entity(info);
        entt::registry& registry = entity_manager.get_registry();
        registry.emplace<CTile>(entity, type);
        registry.emplace<CPosition>(entity, pos);
        registry.emplace<CTexture>(entity, texture);
        registry.emplace<CTickFast>(entity, tick);
        registry.emplace<CComposition>(entity, float(rand() % 50), 0.01f);
        return entity;
    }
    
    inline Syris::texture::Texture2DBundle defaultTextureBundle(){
        return Syris::texture::Texture2DBundle{
            .src = texture::atlas::grass_0,
            .size = {(float)Syris::config::render_tile_size, (float)Syris::config::render_tile_size},
                ///       .color = RAYWHITE,
                ///       .rotation = 0
        };
    }
}
