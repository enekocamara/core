#pragma once
#include "Syris/ecs/EntityManager.hpp"
#include "Sandbox/scene/helper.h"
#include "Sandbox/texture/SandboxTexture.hpp"
#include <glm/glm.hpp>
#include "Sandbox/ecs/Components.h"
#include "Syris/scripts/Export.h"

namespace Sandbox::ecs{
    EXPORT_FUNCTION inline void chicken_tick(Syris::EntityManager &manager, entt::entity chicken, const Syris::engine_time::Time &time)
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
        manager.get_batch_renderer_manager().set_entity(manager.get_registry().ctx().get<SER_ID>().renderer_id, request);
        manager.get_chunk_system()->move_entity(old_pos, pos.pos, chicken);
    }
}
