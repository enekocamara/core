#include "EntityManager.hpp"

namespace Syris{

    struct CreateInfo
    {
        entt::registry &registry;
    };
    EntityManager::EntityManager(CreateInfo info) : m_batch_renderer_manager(info.batch_renderer_manager),
                                                    m_dll(info.dll),
                                                    m_dll_version(info.dll.get_version()){

    }
    EntityManager::~EntityManager(){
        
    }
    void EntityManager::delete_entity(entt::entity entity) {
        if (m_active_mutex.try_lock()){
            if (m_delete_mutex.try_lock()){
                ecs::ChunkedPosition* op_chunked_pos = m_registry.try_get<ecs::ChunkedPosition>(entity);
                if (op_chunked_pos != nullptr){
                    if (m_chunk_system)
                        m_chunk_system->remove_entity(entity, op_chunked_pos->pos);
                    else
                        BREAK_POINT("should be active");
                }
                m_registry.destroy(entity);
                m_delete_mutex.unlock();
            }else{
                auto [queue, lock] = m_delete_queue.get();
                queue.emplace_back(entity);
            }
            m_active_mutex.unlock();
        }else{
            auto [queue, lock] = m_delete_queue.get();
            queue.emplace_back(entity);
        }
    }

    void EntityManager::proccess_queues(){
        std::unique_lock<std::mutex> active_lock(m_active_mutex);
        {
            auto[queue, lock] = m_delete_queue.get();
            for (auto entity : queue){
                ecs::ChunkedPosition* op_chunked_pos = m_registry.try_get<ecs::ChunkedPosition>(entity);
                if (op_chunked_pos != nullptr){
                    if (m_chunk_system)
                        m_chunk_system->remove_entity(entity, op_chunked_pos->pos);
                    else
                        BREAK_POINT("should be active");
                }
                m_registry.destroy(entity);
            }
            queue.clear();
        }
        {
            auto[queue, lock] = m_activate_queue.get();
            for (entt::entity entity : queue){
                if (m_chunk_system)
                    m_chunk_system->add_entity_if_implements_t<ecs::ChunkedPosition>(m_registry, entity);
                m_registry.emplace<ecs::Active>(entity);
            }
            queue.clear();
        }
        {
            auto[queue, lock] = m_exec_function_queue.get();
            for (auto f : queue){
                std::cout << "exec function?\n";
                f(m_registry);
            }
            queue.clear();
        }
    }
    void EntityManager::exec_func(std::function<void(entt::registry&)> f) {
        auto [queue, lock] = m_exec_function_queue.get();
        queue.push_back(f);
    }
    std::optional<ChunkSystem>& EntityManager::get_chunk_system() {
        return m_chunk_system;
    }
    void EntityManager::update_dll(){
        //auto [dll, dll_lock] = m_dll.get();
        m_dll.block_reloading();
        if (m_dll_version != m_dll.get_version()){

            //lock_active_mutex();
            std::unique_lock<std::mutex> lock(m_active_mutex);
            auto tick_group = get_registry().group<Syris::ecs::OnUpdate>();
            for (entt::entity entity: tick_group){
                
                auto& on_update = tick_group.get<Syris::ecs::OnUpdate>(entity);
                if (on_update.dll_name) {
                    auto res = m_dll.get_function_ptr<Syris::ecs::OnUpdate::FunctionType>
                                (std::format("{}_on_update", *on_update.dll_name));
                    ASSERT(res, "cant load dll function");
                    on_update.on_update = res.value();
                }
            }
            //unlock_active_mutex();
        }
    }
}