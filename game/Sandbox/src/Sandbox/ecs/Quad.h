#pragma once
#include <entt.hpp>

#include "Syris/ecs/EntityManager.hpp"
#include "Syris/materials/Material.hpp"

#include "Components.h"

namespace Sandbox::ecs::Quad {
    inline entt::entity newQuad(glm::vec3 color, Syris::EntityManager &entity_manager, Syris::MaterialManager::MaterialID material_id)
    {
        Syris::EntityManager::RenderInfo render_info{
            .material = material_id,
            //.size = sizeof(glm::vec3),
            .entity_data = &color,
        };
        Syris::EntityManager::EntityInfo info{
            .render_info = render_info
        };
        return entity_manager.new_entity(info);
    }
}
