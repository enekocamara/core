#pragma once
#include <atomic>
#include "Syris/utils/EngineTime.hpp"
#include "Syris/utils/FastFunction.hpp"
namespace Syris{
    class EntityManager;
}

namespace Syris::ecs{
    struct MarkedForDestruction{};

    //marks if an entity can be used in the tick group
    struct Active{};

    //to be tracked by the chunk system u must implement
    struct ChunkedPosition{
        glm::vec2 pos;
    };
    struct OnUpdate{
        using FunctionType = void(EntityManager&, entt::entity, const engine_time::Time& time); 
        FastFunction<FunctionType> on_update;
        std::optional<const char*> dll_name;
    };
}

#include "EntityManager.hpp";