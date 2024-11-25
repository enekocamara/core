#include "ER_DynRequestQueue.hpp"
/*
namespace Syris{
        DynRequestQueue::DynRequestQueue(uint64_t type_size){
            m_type_size = type_size;
            m_request_count = 0;
        }
        DynRequestQueue::~DynRequestQueue(){

        }

        void DynRequestQueue::add_request(BR_DynPlainRequest &request){
            if (request.data == nullptr && m_type_size != 0){
                throw std::runtime_error("Request passed with nullptr");
            }
            
            m_request_count++;
            m_entity_to_index[request.entity] = m_request_count;
            m_index_to_entity[m_request_count] = request.entity;
            m_mem.assign(static_cast<std::byte *>(request.data), static_cast<std::byte *>(request.data) + m_type_size);
        }
        bool DynRequestQueue::remove_request(entt::entity entity){
            if (m_entity_to_index.find(entity) == m_entity_to_index.end())
                return false;
            uint64_t index = m_entity_to_index[entity];
            if (index != m_request_count - 1)
            {
                uint64_t index_to_move = m_request_count - 1;

                std::memcpy(m_mem.data() + index * m_type_size, m_mem.data() + index_to_move * m_type_size, m_type_size);
                //  m_mem[index] = m_mem[index_to_move];

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
        bool DynRequestQueue::set_if_find(BR_DynPlainRequest &request)
        {
            auto it = m_entity_to_index.find(request.entity);
            if (it == m_entity_to_index.end())
                return false;
            if (request.data != nullptr) // this needs to be changed asap into its own dataless queue
                std::memcpy(m_mem.data() + it->second * m_type_size, request.data, m_type_size);
                //m_mem[it->second] = *request.data;
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
*/