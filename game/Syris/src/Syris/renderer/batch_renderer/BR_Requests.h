#pragma once
#include <entt.hpp>
#include <span>

namespace Syris
{
    struct BR_Request
    {
        entt::entity entity;
        void *data;
    };

    struct BR_RequestSparse{
        entt::entity entity;
        std::span<std::pair<std::size_t, void *>>data;
    };

    struct BR_RemoveRequest
    {
        entt::entity entity;
    };

    struct BR_SetAttributeRequest{
        std::vector<float> values;
    };
}
            