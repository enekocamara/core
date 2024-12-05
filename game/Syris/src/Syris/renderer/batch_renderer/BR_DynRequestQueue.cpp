#include "BR_DynRequestQueue.hpp"
#include "Syris/utils/Debug.h"
namespace Syris{
        DynRequestQueue::DynRequestQueue(uint64_t type_size){
            m_type_size = type_size;
            m_request_count = 0;
            ASSERT(m_type_size != 0, "Size has to be bigger than 0");
        }
        DynRequestQueue::~DynRequestQueue(){

        }

        void DynRequestQueue::add_request(BR_Request &request){
            ASSERT(request.data != nullptr, "request data cannot be nullptr");
            m_entity_to_index[request.entity] = m_request_count;
            m_index_to_entity[m_request_count] = request.entity;
            const std::byte* src = static_cast<std::byte*>(request.data);
            m_mem.insert(m_mem.end(), src, src + m_type_size);
            m_request_count++;
        }
        bool DynRequestQueue::remove_request(entt::entity entity){
            auto it = m_entity_to_index.find(entity);
            if (it == m_entity_to_index.end())
                return false;
            uint64_t index = it->second;
            if (index != m_request_count - 1)
            {
                uint64_t index_to_move = m_request_count - 1;

                std::memcpy(m_mem.data() + index * m_type_size,
                    m_mem.data() + index_to_move * m_type_size, m_type_size);

                // reasing moved entity values
                entt::entity moved_entity = m_index_to_entity[index_to_move];
                m_entity_to_index[moved_entity] = index;
                m_index_to_entity[index] = moved_entity;

                // remove unused data
                m_entity_to_index.erase(entity);
                m_index_to_entity.erase(index_to_move);
                m_mem.resize(m_mem.size() - m_type_size);
            }
            else
            {
                m_entity_to_index.erase(entity);
                m_index_to_entity.erase(index);
                m_mem.resize(m_mem.size() - m_type_size);
            }
            return true;
        }
        bool DynRequestQueue::set_if_find(BR_Request &request)
        {
            ASSERT(request.data != nullptr, "request data cannot be nullptr");
            auto it = m_entity_to_index.find(request.entity);
            if (it == m_entity_to_index.end())
                return false;
            std::memcpy(m_mem.data() + it->second * m_type_size, request.data, m_type_size);
            return true;
        }
        void DynRequestQueue::clear()
        {
            m_mem.clear();
            m_entity_to_index.clear();
            m_index_to_entity.clear();
            m_request_count = 0;
        };

}
