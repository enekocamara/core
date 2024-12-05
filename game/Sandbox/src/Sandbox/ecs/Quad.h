#pragma once
#include <entt.hpp>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/renderer/batch_renderer/BatchRenderer.hpp"

#include "Components.h"

namespace Sandbox::ecs::Quad {
    inline entt::entity newQuad(glm::vec3 color, Syris::EntityManager &entity_manager, Syris::BatchRendererManager::BR_ID material_id)
    {
        std::array<std::pair<std::size_t, void*>, 1>entity_data;
        entity_data[0] = { 0, &color };
        Syris::EntityManager::RenderInfo render_info{
            .renderer = material_id,
            //.size = sizeof(glm::vec3),
            .request = {entity_data.begin(), entity_data.end()},
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info
        };
        BREAK_POINT("TODO!");
        //return entity_manager.new_entity(info);
    }
}
