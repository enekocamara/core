#pragma once
#include <atomic>

namespace Syris::ecs{
    struct MarkedForDestruction{};

    //marks if an entity can be used in the tick group
    struct Active{};

    //to be tracked by the chunk system u must implement
    struct ChunkedPosition{
        glm::vec2 pos;
    };
}