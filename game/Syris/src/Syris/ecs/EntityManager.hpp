#pragma once

#include <mutex>
#include <optional>
#include <entt.hpp>
#include <condition_variable>
#include <vector>
#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"
#include "Syris/utils/containers/MutexGuard.hpp"
#include "Components.h"
#include "Syris/utils/world_generation/ChunkSystem.hpp"

namespace Syris{
    class EntityManager{
        public:
            struct RenderInfo{
                BatchRendererManager::BR_ID renderer;
                std::span<std::pair<std::size_t, void*>> request;
            };
            struct EntityInfo{
                std::optional<RenderInfo> render_info;
            };
            struct CreateInfo{
                BatchRendererManager& batch_renderer_manager;
            };

            EntityManager(CreateInfo info);
            ~EntityManager();

            EntityManager() = delete;
            EntityManager(const EntityManager& ref) = delete;
            EntityManager& operator=(const EntityManager& ref) = delete;

            entt::entity new_entity(EntityInfo& info, std::function<void(entt::entity)> f) {
                m_create_mutex.lock();
                entt::entity id = m_registry.create();
                //f(id, m_registry);
                if (m_active_mutex.try_lock()){
                    m_registry.emplace<ecs::Active>(id);
                    f(id);
                    ecs::ChunkedPosition *op_chunked_pos = m_registry.try_get<ecs::ChunkedPosition>(id);
                    if (op_chunked_pos != nullptr)
                    {
                        if (m_chunk_system)
                            m_chunk_system->add_entity(id, op_chunked_pos->pos);
                        else
                            BREAK_POINT("should be active");
                    }
                    m_active_mutex.unlock();
                }else{
                    auto[queue, lock] = m_activate_queue.get();
                    queue.push_back({id, f});
                }
                m_create_mutex.unlock();
                if (info.render_info.has_value()) {
                    RenderInfo render = info.render_info.value();                    
                    BR_RequestSparse request{
                        .entity = id,
                        .data = render.request
                    };
                    m_batch_renderer_manager.get_renderer(render.renderer)->add_entity(request);
                }
                return id;
            }
            void delete_entity(entt::entity entity);
            entt::registry& get_registry(){return m_registry;}
            Syris::BatchRendererManager& get_batch_renderer_manager() {return m_batch_renderer_manager;}
            void lock_active_mutex(){m_active_mutex.lock();}
            void unlock_active_mutex(){m_active_mutex.unlock();proccess_queues();};
            std::unique_lock<std::mutex> get_active_lock(){return std::unique_lock(m_active_mutex);}
            void exec_func(std::function<void(entt::registry&)> f);
            template<typename... Args>
            void create_chunk_system(Args&&... args){
                m_chunk_system.emplace(std::forward<Args>(args)...);
            }
            std::optional<ChunkSystem>& get_chunk_system();
        private:
            void proccess_queues();
            entt::registry m_registry;
            uint32_t m_entity_count;

            std::mutex m_delete_mutex;
            std::mutex m_create_mutex;

            std::mutex m_active_mutex;
            
            MutexGuard<std::vector<entt::entity>> m_delete_queue;
            MutexGuard<std::vector<std::function<void(entt::registry&)>>> m_exec_function_queue;
            MutexGuard<std::vector<std::pair<entt::entity, std::function<void(entt::entity)>>>> m_activate_queue;
            
            BatchRendererManager& m_batch_renderer_manager;
            std::condition_variable m_cond_var;

            std::optional<ChunkSystem> m_chunk_system = std::nullopt;
    };
}