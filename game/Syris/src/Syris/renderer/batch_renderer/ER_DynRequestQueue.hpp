#pragma once
#include <vector>
#include <unordered_map>
#include <entt.hpp>
#include "Syris/utils/memory.h"
#include "ER_DynRequests.h"
#include "Syris/types/Type.h"
/*
namespace Syris{
    struct DynQueue {
        std::vector<std::byte>& m_request_data;
        uint64_t m_type_size;
        std::unordered_map<entt::entity, uint64_t>& m_entity_to_index;
    };

    class DynRequestQueue
    {
    public:
        DynRequestQueue(uint64_t type_size);
        ~DynRequestQueue();
        DynRequestQueue(const DynRequestQueue &ref) = delete;
        DynRequestQueue &operator=(const DynRequestQueue &ref) = delete;

        void add_request(BR_DynPlainRequest &request);
        bool remove_request(entt::entity entity);
        DynQueue get_queue() { return {m_mem, m_type_size, m_entity_to_index}; }
        bool set_if_find(BR_DynPlainRequest &request);
        void clear();

    private:
        std::vector<std::byte> m_mem;
        uint64_t m_type_size;
        uint64_t m_request_count;
        std::unordered_map<entt::entity, uint64_t> m_entity_to_index;
        std::unordered_map<uint64_t, entt::entity> m_index_to_entity;
    };
}
*/