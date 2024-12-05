#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <set>
#include <tuple>

#include <entt.hpp>

#include "BatchRenderer.hpp"
#include "BR_RequestQueue.hpp"
#include "BR_RequestQueueGroup.hpp"
#include "BR_Requests.h"
#include "Syris/utils/containers/MutexGuard.hpp"
#include "Syris/utils/std/meta.h"


namespace Syris{

    template<typename T>
    concept GetQueue = requires(BR_RequestQueue<T> t){
        {t.get_queue()} -> std::same_as<BR_Queue<T>>;
    };
    template<GetQueue T>
    auto get_queue(T& queue) {
        return queue.get_queue();
    }

    /** @brief Manages requests in a thread safe way by pooling request */
    class BR_RequestQueueSystem
    {
    public:
        BR_RequestQueueSystem(std::vector<std::size_t> sizes)
            : m_sizes(sizes),
              m_add_queues(sizes),
              m_set_queues(sizes){

        }
        BR_RequestQueueSystem() = delete;
        ~BR_RequestQueueSystem() {}

        BR_RequestQueueSystem(const BR_RequestQueueSystem &ref) = delete;
        BR_RequestQueueSystem &operator=(const BR_RequestQueueSystem &ref) = delete;

        void add_entity(BR_RequestSparse &request){
            
            auto[queues, lock] = m_add_queues.get();
            queues.add_request(request);
        }

        /**
         * @brief Combines the append request that go to the same subbufer to reduce calls
            its size is not representative of the number of append requests done.
            It will lock the append queue until clear_append_queue is called.
           @return An iterable struct with all the append requests in the queue
        */
        std::pair<RequestQueues, std::unique_lock<std::mutex>> get_add_queue(){
            auto [add_queues,lock] = m_add_queues.get(); 
            return {add_queues.get_queues(), std::move(lock)};
        }


        /** @brief Clear all the data related to append requests and unlock the append queue*/
        void clear_add_queue(std::unique_lock<std::mutex>&& add_lock){
            auto& queues = m_add_queues.get_unguarded();
            queues.clear();
       }

        /**
         * @brief Stores the set request in the set queue, all memory being stored in a single
            memqueue
        */
        void set_entity(BR_RequestSparse& request)
        {
            auto [set_queues, set_lock] = m_set_queues.get();
            auto [add_queues, add_lock] = m_add_queues.get();
            //m_add_mutex.lock();
            std::vector<std::pair<std::size_t, void *>> unset = add_queues.set_if_find(request);
            set_queues.add_request(request);
            /*
            for (auto [queue_index, data] : request.data) {
                BR_Request typed_request{ request.entity, data };
                if (!add_queue[].set_if_find(typed_request))
                    set_queue.add_request(typed_request);
            }*/
            
            //m_add_mutex.unlock();
            //m_set_mutex.unlock();
        }



        /**
         * @return Returns a iterable struct holding all the set request. It will lock the set queue
         * until clear_set_queue is called
         */
        std::pair<RequestQueues, std::unique_lock<std::mutex>> get_set_queue()
        {
            auto [set_queues, lock] = m_set_queues.get();
            return { set_queues.get_queues(), std::move(lock) };
            //return { queue.get_queue(), std::move(lock) };
            /*m_set_mutex.lock();
            return m_set_queue.get_queue();*/
        }

        // uint32_t get_set_request_count() { return m_set_queue.m_requests.size(); }

        void clear_set_queue(std::unique_lock<std::mutex>&& set_lock)
        {
            m_set_queues.get_unguarded().clear();
            //m_set_queue.clear();
            //m_set_mutex.unlock();
        }

        void remove_entity(BR_RemoveRequest &request){
            
            auto [remove_queue, remove_lock] = m_remove_queue.get();
            auto [set_queue, set_lock] = m_set_queues.get();
            if (set_queue.remove_request(request.entity))
                remove_queue.push_back(request.entity);
            else{
                auto [add_queue, add_lock] = m_add_queues.get();
                if (!add_queue.remove_request(request.entity))
                   remove_queue.push_back(request.entity);
            }
        }

        std::pair<std::vector<entt::entity>&, std::unique_lock<std::mutex>> get_remove_queue(){
            auto [queue, lock] = m_remove_queue.get();
            return { queue, std::move(lock) };
            //m_remove_mutex.lock();
            //return m_remove_queue;
        }

        // uint32_t get_remove_request_count(){return m_remove_requests.m_requests.size();}

        void clear_remove_queue(std::unique_lock<std::mutex>&& remove_lock){
            m_remove_queue.get_unguarded().clear();
            /*m_remove_queue.clear();
            m_remove_mutex.unlock();*/
        }

        void set_attributes(const std::vector<float>& values){
            //m_attributes_mutex.lock();
            auto [attributes, lock] = m_attributes.get();
            attributes = values;
            //m_attributes = values;
            //m_attributes_mutex.unlock();
        }
        
        std::pair<std::vector<float>&, std::unique_lock<std::mutex>> get_attributes(){
            //m_attributes_mutex.lock();
            return m_attributes.get();
        }
        void clear_attributes(std::unique_lock<std::mutex>&& attributes_lock){
            m_attributes.get_unguarded().clear();
            //m_attributes_mutex.unlock();
        }
    private:
        std::vector<std::size_t> m_sizes;
        MutexGuard<BR_RequestQueueGroup<BR_Request>> m_add_queues;
        MutexGuard<BR_RequestQueueGroup<BR_RequestSparse>> m_set_queues;
        MutexGuard<std::vector<entt::entity>> m_remove_queue;
        MutexGuard<std::vector<float>> m_attributes;

        //RequestQueue<T> m_remove_queue;

        /*std::mutex m_set_mutex;
        std::mutex m_add_mutex;
        std::mutex m_remove_mutex;
        std::mutex m_attributes_mutex;*/
    };
}
