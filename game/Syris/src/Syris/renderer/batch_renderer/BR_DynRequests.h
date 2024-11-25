#pragma once
#include <entt.hpp>
#include "Syris/types/Type.h"

namespace Syris
{
    struct BR_DynPlainRequest
    {
        entt::entity entity;
        void* data;
    };
    struct BR_DynSetAttributeRequest{
        void *data;
        uint64_t size;
        //std::vector<DynVar> data;
    };
    struct BR_DynRequest
    {
        entt::entity entity;
        //DynTypeValList* data;
        void* data;
    };

    struct BR_DynSetRequest
    {
        entt::entity entity;
        void *data;
    };

    struct BR_DynAddRequest
    {
        entt::entity entity;
        //DynTypeValList* data;
        void* data;
        //std::vector<DynVar> vars;
    };

    struct BR_DynRemoveRequest
    {
        entt::entity entity;
    };
}
            