#pragma once
#include "Syris/ecs/EntityManager.hpp"
#include "Sandbox/scene/helper.h"
#include "Sandbox/texture/SandboxTexture.hpp"
#include <glm/glm.hpp>
#include "Sandbox/ecs/Components.h"
#include "Syris/scripts/Export.h"

namespace Sandbox::ecs{
    EXPORT_FUNCTION inline void chicken_on_update(Syris::EntityManager &manager, entt::entity chicken, const Syris::engine_time::Time &time)
    {
        auto [pos, move_speed, current_speed, dir] = manager.get_registry().get<Syris::ecs::ChunkedPosition, CMovementSpeed, CSpeed, CDir>(chicken);
        glm::vec2 old_pos = pos.pos;
        pos.pos += time.get_delta_ms() * (current_speed.speed / 1000.f) * dir.value;

        QuadTexInstancedData instance_data = Sandbox::QuadTexInstancedData();
        instance_data.tex_coord = {texture::atlas::chicken.min, texture::atlas::chicken.max};
        instance_data.translation = glm::translate(instance_data.translation, glm::vec3(pos.pos, 1.f));

        // BREAK_POINT("TODO!");
        std::array<std::pair<size_t, void *>, 1> entity_data;
        entity_data[0] = {0, &instance_data};
        Syris::BR_RequestSparse request{
            .entity = chicken,
            .data = {entity_data.begin(), entity_data.end()} //&instance_data
        };
        manager.get_batch_renderer_manager().set_entity(manager.get_registry().ctx().get<entities::SER_ID>().renderer_id, request);
        manager.get_chunk_system()->move_entity(old_pos, pos.pos, chicken);
    }

    EXPORT_FUNCTION inline void /*std::tuple<QuadTexInstancedData>*/ chicken_on_create(Syris::EntityManager &entity_manager, entt::entity chicken,std::tuple<QuadTexInstancedData>& render_data, glm::vec2 pos)
    {
        // this shuld be backed in at least part of it
        //static QuadTexInstancedData instance_data{
        std::get<0>(render_data).tex_coord = {texture::atlas::chicken.min, texture::atlas::chicken.max},
        std::get<0>(render_data).translation = glm::translate(glm::mat4(1.f), glm::vec3(pos, 1.f));
        // how do we get pos??
        //instance_data.translation ;

        entt::registry &registry = entity_manager.get_registry();
        registry.emplace<Syris::ecs::ChunkedPosition>(chicken, pos);
        registry.emplace<CSpeed>(chicken, glm::vec2(0.1f));
        registry.emplace<CMovementSpeed>(chicken, 3.f);
        registry.emplace<CDir>(chicken, glm::vec2(0.f, 1.f));
        // registry.emplace<CTickFast>(chicken, res.value());
        //return {instance_data};
    }
}
