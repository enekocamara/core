#pragma once
#include "ChunkSystem.hpp"
#include "Syris/utils/Debug.h"

namespace Syris{
    void ChunkSystem::add_entity(entt::entity entity, glm::vec2 world_position){
        auto [queue, lock] = m_new_entities.get();
        std::size_t chunk_index = get_chunk_index_from_world_pos(world_position);
        queue.emplace_back(chunk_index, entity);
    }

    void ChunkSystem::remove_entity(entt::entity entity, glm::vec2 world_position){
        std::size_t chunk_index = get_chunk_index_from_world_pos(world_position);
        auto [queue, lock] = m_remove_entity.get();
        queue.emplace_back(chunk_index, entity);
    }


    std::size_t ChunkSystem::get_chunk_index_from_world_pos(glm::vec2 world_position){
        glm::uvec2 chunk_pos = (glm::uvec2)(world_position / m_tile_size) / m_chunk_size;
        return get_chunk_index_from_chunk_pos(chunk_pos);
    }
    std::size_t ChunkSystem::get_chunk_index_from_chunk_pos(glm::uvec2 chunk_position){
        return chunk_position.y * m_chunk_dimmensions.x + chunk_position.x;
    }
    void ChunkSystem::sync(){
        auto [new_entities, new_entities_lock] =  m_new_entities.get();
        auto [pos_changes, pos_changes_lock] = m_pos_changes.get();
        auto [remove_entity, remove_entity_lock] = m_remove_entity.get();
        for (auto [chunk_index, entity] : new_entities){
            m_chunks[chunk_index].m_entities.push_back(entity);
        }
        for (auto [from, to, entity] : pos_changes){
            auto it = std::find(m_chunks[from].m_entities.begin(), m_chunks[from].m_entities.end(), entity);
            ASSERT(it != m_chunks[from].m_entities.end(), "entity was not where it was supposed to be");
            m_chunks[from].m_entities.erase(it);
            m_chunks[to].m_entities.push_back(entity);
        }
        for (auto [chunk_index, entity] : remove_entity){
            auto it = std::find(m_chunks[chunk_index].m_entities.begin(), m_chunks[chunk_index].m_entities.end(), entity);
            ASSERT(it != m_chunks[chunk_index].m_entities.end(), "entity was not where it was supposed to be");
            m_chunks[chunk_index].m_entities.erase(it);
        }
    }
    void ChunkSystem::move_entity(glm::vec2 old_world_pos, glm::vec2 new_world_pos, entt::entity entity){
        std::size_t old_chunk = get_chunk_index_from_world_pos(old_world_pos);
        std::size_t new_chunk = get_chunk_index_from_world_pos(new_world_pos);
        if (old_chunk != new_chunk){
            auto [queue, lock] = m_pos_changes.get();
            queue.emplace_back(old_chunk, new_chunk, entity);
        }
    }
}