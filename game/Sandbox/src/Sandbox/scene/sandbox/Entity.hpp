#pragma once

#include <entt.hpp>

namespace Sandbox{

    template<typename ...Args>
    class Entity{
        public:
            Entity(entt::registry& registry, Args&& ...args) : m_id(registry.create()){
                (registry.emplace<Args>(m_id, std::forward<Args>(args)),...);
            }
        private:
        entt::entity m_id;
    };
}