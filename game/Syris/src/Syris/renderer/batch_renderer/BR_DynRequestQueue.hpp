#pragma once
#include <vector>
#include <unordered_map>
#include <entt.hpp>
#include "Syris/utils/memory.h"
#include "BR_Requests.h"
#include "Syris/types/Type.h"

namespace Syris{
    struct DynQueue {
        std::vector<std::byte>& m_request_data;
        std::unordered_map<entt::entity, uint64_t>& m_entity_to_index;
        uint64_t m_type_size;
    };

    class DynRequestQueue
    {
    public:
        DynRequestQueue(uint64_t type_size);
        ~DynRequestQueue();
        DynRequestQueue(const DynRequestQueue &ref) = delete;
        DynRequestQueue &operator=(const DynRequestQueue &ref) = delete;
        DynRequestQueue(DynRequestQueue&&) noexcept = default;

        void add_request(BR_Request &request);
        bool remove_request(entt::entity entity);
        DynQueue get_queue() { return {m_mem, m_entity_to_index, m_type_size}; }
        bool set_if_find(BR_Request &request);
        void clear();

    private:
        std::vector<std::byte> m_mem;
        uint64_t m_type_size;
        uint64_t m_request_count = 0;
        std::unordered_map<entt::entity, uint64_t> m_entity_to_index;
        std::unordered_map<uint64_t, entt::entity> m_index_to_entity;
    };
}