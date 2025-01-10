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
#include "Syris/utils/std/meta.h"
#include "Syris/scripts/DynamicLibraryLoader.hpp"

namespace Syris{
    class EntityManager {
    public:
        struct RenderInfo
        {
            BatchRendererManager::BR_ID renderer;
            using RequestType = std::span<std::pair<std::size_t, void *>>;
            template <std::size_t size>
            using RequestTypeArray = std::array<std::pair<std::size_t, void *>, size>;
            RequestType request;
        };
        struct EntityInfo
        {
            std::optional<RenderInfo> render_info;
        };
        struct CreateInfo
        {
            BatchRendererManager &batch_renderer_manager;
            DynamicLibraryLoader &dll;
        };

        EntityManager(CreateInfo info);
        ~EntityManager();

        EntityManager() = delete;
        EntityManager(const EntityManager &ref) = delete;
        EntityManager &operator=(const EntityManager &ref) = delete;

        template <typename OnCreate, typename... Args>
        entt::entity new_entity(BatchRendererManager::BR_ID renderer, OnCreate on_create, Args &&...args)
        {
            using RenderTuple = typename meta::FunctionPointer<OnCreate>::ReturnType;
            std::unique_lock create_lock(m_create_mutex);
            entt::entity id = m_registry.create();

            if constexpr (std::is_same_v<RenderTuple, void>)
            {
                on_create(*this, id, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = nullptr};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }
            else
            {
                RenderTuple render_data = on_create(*this, id, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = &render_data};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }

            if (m_active_mutex.try_lock())
            {
                if (m_chunk_system)
                    m_chunk_system->add_entity_if_implements_t<ecs::ChunkedPosition>(m_registry, id);
                m_registry.emplace<ecs::Active>(id);
                m_active_mutex.unlock();
            }
            else
            {
                auto [queue, lock] = m_activate_queue.get();
                queue.push_back(id);
            }
            return id;
        }
        template <typename OnCreate, typename... Args>
        entt::entity new_entity_dll(BatchRendererManager::BR_ID renderer, const char *name, Args &&...args)
        {
            using RenderTuple = typename meta::FunctionPointer<OnCreate>::ReturnType;
            std::unique_lock create_lock(m_create_mutex);
            entt::entity id = m_registry.create();

            //auto [dll, dll_lock] = m_dll.get();
            auto on_update = m_dll.get_function_ptr<ecs::OnUpdate::FunctionType>(std::format("{}_on_update", name));
            if (!on_update)
                BREAK_POINT(std::format("Couldn't find dll function {}_on_update", name));

            m_registry.emplace<ecs::OnUpdate>(id, on_update.value());

            auto on_create = m_dll.get_function_ptr<OnCreate>(std::format("{}_on_create", name));
            if (!on_create)
                BREAK_POINT(std::format("Couldn't find dll function {}_on_create", name));

            if constexpr (std::is_same_v<RenderTuple, void>)
            {
                on_create.value()(*this, id, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = nullptr};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }
            else
            {
                RenderTuple render_data = on_create.value()(*this, id, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = &render_data};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }
            if (m_active_mutex.try_lock())
            {
                if (m_chunk_system)
                    m_chunk_system->add_entity_if_implements_t<ecs::ChunkedPosition>(m_registry, id);
                m_registry.emplace<ecs::Active>(id);
                m_active_mutex.unlock();
            }
            else
            {
                auto [queue, lock] = m_activate_queue.get();
                queue.push_back(id);
            }
            return id;
        }
        template <typename RenderTuple, typename... Args>
        entt::entity new_entity_dll_better(BatchRendererManager::BR_ID renderer, const char *name, Args...args)
        {
            std::unique_lock create_lock(m_create_mutex);
            entt::entity id = m_registry.create();
            //auto [dll, dll_lock] = m_dll.get();
            auto on_update = m_dll.get_function_ptr<ecs::OnUpdate::FunctionType>(std::format("{}_on_update", name));
            if (!on_update)
                BREAK_POINT(std::format("Couldn't find dll function {}_on_update", name));

            m_registry.emplace<ecs::OnUpdate>(id, on_update.value(), std::optional(name));

            if constexpr (std::is_same_v<RenderTuple, void>)
            {
                using OnCreate = void(EntityManager &, entt::entity, Args...);
                auto on_create = m_dll.get_function_ptr<OnCreate>(std::format("{}_on_create", name));
                if (!on_create)
                    BREAK_POINT(std::format("Couldn't find dll function {}_on_create", name));

                on_create.value()(*this, id, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = nullptr};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }
            else
            {
                using OnCreate = void(EntityManager &, entt::entity, RenderTuple &, Args...);
                auto on_create = m_dll.get_function_ptr<OnCreate>(std::format("{}_on_create", name));
                if (!on_create)
                    BREAK_POINT(std::format("Couldn't find dll function {}_on_create", name));

                RenderTuple render_data;
                on_create.value()(*this, id, render_data, std::forward<Args>(args)...);
                BR_Request request{
                    .entity = id,
                    .data = &render_data};
                m_batch_renderer_manager.get_renderer(renderer)->add_entity(request);
            }

            if (m_active_mutex.try_lock())
            {
                if (m_chunk_system)
                    m_chunk_system->add_entity_if_implements_t<ecs::ChunkedPosition>(m_registry, id);
                m_registry.emplace<ecs::Active>(id);
                m_active_mutex.unlock();
            }
            else
            {
                auto [queue, lock] = m_activate_queue.get();
                queue.push_back(id);
            }
            return id;
        }

        /*
        entt::entity new_entity(EntityInfo& info, std::function<void(entt::entity)> f) {
            TODO();

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

            return (entt::entity)0;
        }*/
        void delete_entity(entt::entity entity);
        entt::registry &get_registry() { return m_registry; }
        Syris::BatchRendererManager &get_batch_renderer_manager() { return m_batch_renderer_manager; }
        void lock_active_mutex() { m_active_mutex.lock(); }
        void unlock_active_mutex()
        {
            m_active_mutex.unlock();
            proccess_queues();
        };
        std::unique_lock<std::mutex> get_active_lock() { return std::unique_lock(m_active_mutex); }
        void exec_func(std::function<void(entt::registry &)> f);
        template <typename... Args>
        void create_chunk_system(Args &&...args)
        {
            m_chunk_system.emplace(std::forward<Args>(args)...);
        }
        std::optional<ChunkSystem> &get_chunk_system();

        void update_dll();
        DynamicLibraryLoader& get_dll() {return m_dll;}

    private:
        void proccess_queues();

        BatchRendererManager &m_batch_renderer_manager;
        DynamicLibraryLoader &m_dll;
        uint64_t m_dll_version;

        entt::registry m_registry;
        uint32_t m_entity_count;

        std::mutex m_delete_mutex;
        std::mutex m_create_mutex;

        std::mutex m_active_mutex;

        MutexGuard<std::vector<entt::entity>> m_delete_queue;
        MutexGuard<std::vector<std::function<void(entt::registry &)>>> m_exec_function_queue;
        MutexGuard<std::vector<entt::entity>> m_activate_queue;

        std::condition_variable m_cond_var;

        std::optional<ChunkSystem> m_chunk_system = std::nullopt;
    };
}