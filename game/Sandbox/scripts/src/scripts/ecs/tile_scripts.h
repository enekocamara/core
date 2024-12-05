#pragma once
#include <entt.hpp>
#include "Syris/ecs/EntityManager.hpp"
#include "Syris/scripts/DynamicScriptLoader.hpp"
#include "Syris/utils/EngineTime.hpp"

#include "Sandbox/world_generator/World.hpp"

#define DLSCRIPT extern "C" __declspec(dllexport)

DLSCRIPT void tile_tick(Syris::EntityManager &entity_manager, entt::entity entity, const Syris::engine_time::Time &time){
    
}
