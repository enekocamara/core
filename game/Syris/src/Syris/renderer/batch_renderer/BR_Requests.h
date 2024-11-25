#pragma once
#include <entt.hpp>

namespace Syris
{
    template <typename T>
    struct BR_Request
    {
        entt::entity entity;
        T *data;
    };

    struct BR_SetRequest
    {
        entt::entity entity;
        void *data;
    };

    struct BR_AddRequest
    {
        entt::entity entity;
        void *data;
    };

    struct BR_RemoveRequest
    {
        entt::entity entity;
    };

    struct BR_SetAttributeRequest{
        std::vector<float> values;
    };
}
            