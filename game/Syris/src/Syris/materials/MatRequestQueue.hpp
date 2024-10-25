#pragma once
#include <vector>
#include "Syris/utils/memory.h"
#include <unordered_map>
#include "Syris/materials/Material.hpp"

namespace Syris{
    template<typename T>
    struct Queue {
        std::vector<T>& m_request_data;
        std::unordered_map<entt::entity, uint64_t>& m_entity_to_index;
    };

    template<typename T>
    struct RequestQueue
    {
        void add_request(MaterialRequest<T> &request)
        {
            /*if (m_entity_to_index.find(request.entity) != m_entity_to_index.end())
            {
                CORE_ERROR("Adding entity twice");
                exit(1);
            }*/
            m_entity_to_index[request.entity] = m_request_data.size();
            m_index_to_entity[m_request_data.size()] = request.entity;
            if (request.data == nullptr)
                m_request_data.push_back(T());
            else
                m_request_data.push_back(*request.data);
        }
        bool remove_request(entt::entity entity){
            if (m_entity_to_index.find(entity) == m_entity_to_index.end())
                return false;
            uint64_t index = m_entity_to_index[entity];
            if (m_request_data.size() != 1)
            {
                uint64_t index_to_move = m_request_data.size() - 1;

                // copy data
                m_request_data[index] = m_request_data[index_to_move];

                // reasing moved entity values
                entt::entity moved_entity = m_index_to_entity[index_to_move];
                m_entity_to_index[moved_entity] = index;
                m_index_to_entity[index] = moved_entity;

                // remove unused data
                m_entity_to_index.erase(entity);
                m_index_to_entity.erase(index_to_move);
                m_request_data.resize(m_request_data.size() - 1);
            }
            else
            {
                uint64_t index = m_entity_to_index[entity];
                m_entity_to_index.erase(entity);
                m_index_to_entity.erase(index);
                m_request_data.resize(0);
            }
            return true;
        }
        Queue<T> get_queue() { return { m_request_data, m_entity_to_index }; }
        bool set_if_find(MaterialRequest<T>& request) {
            auto it = m_entity_to_index.find(request.entity);
            if (it == m_entity_to_index.end())
                return false;
            if (request.data != nullptr)//this needs to be changed asap into its own dataless queue
                m_request_data[it->second] = *request.data;
            return true;
        }
        void clear() {
            m_request_data.clear();
            m_entity_to_index.clear();
            m_index_to_entity.clear();
        };
        std::vector<T> m_request_data;
        std::unordered_map<entt::entity, uint64_t> m_entity_to_index;
        std::unordered_map<uint64_t, entt::entity> m_index_to_entity;
    };
}