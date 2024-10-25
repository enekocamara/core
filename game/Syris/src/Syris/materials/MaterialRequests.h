#pragma once
#include <entt.hpp>

namespace Syris
{
    template <typename T>
    struct MaterialRequest
    {
        entt::entity entity;
        T *data;
    };

    struct MaterialSetRequest
    {
        entt::entity entity;
        void *data;
    };

    struct MaterialAddRequest
    {
        entt::entity entity;
        void *data;
    };

    struct MaterialRemoveRequest
    {
        entt::entity entity;
    };
}
            