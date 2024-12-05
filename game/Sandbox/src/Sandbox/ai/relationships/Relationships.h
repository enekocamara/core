#pragma once

#include <cinttypes>
#include <entt/entt.hpp>
#include <functional>

namespace Sandbox{
    enum Size : uint8_t{
       Tiny,
       Small,
       Normal,
       Big,
       Huge
    };

    enum Type : uint8_t{
        Tree,
        House,
        Rock
    };

    enum State : uint8_t{
        Dry,
        Wet,
        OnFire,
        UnderWater,
        Dead,
        Alive
    };

    struct Relationship{
        Size size;
        Type type;
    };

    struct Ownership{
        Type type;
        entt::entity entity;
    };

    struct Status{
        State state;
    };

    struct GetRelationship{
        std::function<Relationship(entt::entity, entt::registry)> func;
    };

    struct GetOwnership{
        std::function<Ownership(entt::entity, entt::registry)> func;
    };

    struct GetStatus{
        std::function<Status(entt::entity, entt::registry)> func;
    };

    //jons huge tree is on fire
    //jons huge tree is dead

    //First lets get all the information we can
    //get tree id and query for all the data
}