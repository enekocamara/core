#pragma once
#include <entt.hpp>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/renderer/batch_renderer/BatchRenderer.hpp"

#include "Components.h"

namespace Sandbox::ecs::Quad {
    inline entt::entity newQuad(glm::vec3 color, Syris::EntityManager &entity_manager, Syris::BatchRendererManager::BR_ID material_id)
    {
        Syris::EntityManager::RenderInfo render_info{
            .renderer = material_id,
            //.size = sizeof(glm::vec3),
            .entity_data = &color,
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info
        };
        return entity_manager.new_entity(info);
    }
}
