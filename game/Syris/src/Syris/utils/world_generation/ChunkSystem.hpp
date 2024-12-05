#pragma once
#include "Syris/utils/containers/GridLookUp.hpp"
#include "Chunk.hpp"
#include "Syris/utils/containers/MutexGuard.hpp"

namespace Syris{
    class ChunkSystem{
        private:
            struct MovedEntity{
                std::size_t from;
                std::size_t to;
                entt::entity entity;
            };
        public:
            ChunkSystem(glm::uvec2 chunks, glm::uvec2 chunk_sizes, glm::vec2 tile_size):m_chunks(chunks){
            }
            ~ChunkSystem(){}
            ChunkSystem() = delete;
            ChunkSystem(const ChunkSystem& ChunkSystem) = delete;
            ChunkSystem& operator=(const ChunkSystem& ChunkSystem) = delete;
            ChunkSystem(ChunkSystem&& moved) = default;

            const GridLookUp<Chunk> get_chunks()const;
            const GridLookUp<Chunk> get_chunk(glm::vec2 world_position)const;
            const GridLookUp<Chunk> get_chunk(glm::uvec2 chunk_position)const;
            const GridLookUp<Chunk> get_chunk(std::size_t chunk_id)const;

            //thread safe, it will poll all request and execute them with sync
            void move_entity(glm::vec2 old_world_pos, glm::vec2 new_world_pos, entt::entity); 
            void add_entity(entt::entity entity, glm::vec2 world_position);

            void remove_entity(entt::entity entity);
            void remove_entity(entt::entity entity, glm::vec2 world_position);
            void remove_entity(entt::entity entity, glm::uvec2 chunk_position);
            void remove_entity(entt::entity entity, std::size_t chunk_id);

            std::size_t get_chunk_index_from_world_pos(glm::vec2 world_position);
            std::size_t get_chunk_index_from_chunk_pos(glm::uvec2 chunk_position);
            
            //not thread safe, will update entities current chunck
            void sync();

        private:
            GridLookUp<Chunk> m_chunks;
            MutexGuard<std::vector<MovedEntity>> m_pos_changes;
            MutexGuard<std::vector<std::pair<std::size_t, entt::entity>>> m_new_entities;
            MutexGuard<std::vector<std::pair<std::size_t, entt::entity>>> m_remove_entity;
            glm::uvec2 m_chunk_dimmensions;
            glm::uvec2 m_chunk_size;
            glm::vec2 m_tile_size;
            //make a queue for changes?
            //make a refresh that syncs it
    };
}