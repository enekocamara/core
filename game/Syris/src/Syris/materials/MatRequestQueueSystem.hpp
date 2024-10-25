#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <set>

#include <entt.hpp>
#include "Material.hpp"
#include "MatRequestQueue.hpp"


namespace Syris{

    /** @brief Manages requests in a thread safe way by pooling request */
    template<typename T>
    class MatRequestQueueSystem
    {
    public:
        MatRequestQueueSystem() {}
        ~MatRequestQueueSystem() {}

        MatRequestQueueSystem(const MatRequestQueueSystem &ref) = delete;
        MatRequestQueueSystem(MatRequestQueueSystem &ref) = delete;
        MatRequestQueueSystem &operator=(const MatRequestQueueSystem &ref) = delete;

        void add_entity(MaterialAddRequest &request)
        {
            m_add_mutex.lock();
            MaterialRequest<T> typed_request{ request.entity, static_cast<T*>(request.data) };
            m_add_queue.add_request(typed_request);
            m_add_mutex.unlock();
        }

        /**
         * @brief Combines the append request that go to the same subbufer to reduce calls
            its size is not representative of the number of append requests done.
            It will lock the append queue until clear_append_queue is called.
           @return An iterable struct with all the append requests in the queue
        */
        Queue<T> get_add_queue(){
            m_add_mutex.lock();
            return m_add_queue.get_queue();
        }
        // vector of pure data, a pointer. and a map of entity to index

        // uint32_t get_add_request_count() { return m_add_requests.m_requests_count; }

        /** @brief Clear all the data related to append requests and unlock the append queue*/
        void clear_add_queue(){
            m_add_queue.clear();
            m_add_mutex.unlock();
        }

        /**
         * @brief Stores the set request in the set queue, all memory being stored in a single
            memqueue
        */
        void set_entity(MaterialSetRequest &request)
        {
            m_set_mutex.lock();
            m_add_mutex.lock();
            MaterialRequest<T> typed_request{ request.entity, static_cast<T*>(request.data) };
            if (!m_add_queue.set_if_find(typed_request))
            {
                
                m_set_queue.add_request(typed_request);
            }
            m_add_mutex.unlock();
            m_set_mutex.unlock();
        }

        /**
         * @return Returns a iterable struct holding all the set request. It will lock the set queue
         * until clear_set_queue is called
         */
        Queue<T> get_set_queue()
        {
            m_set_mutex.lock();
            return m_set_queue.get_queue();
        }

        // uint32_t get_set_request_count() { return m_set_queue.m_requests.size(); }

        void clear_set_queue()
        {

            m_set_queue.clear();
            m_set_mutex.unlock();
        }

        void remove_entity(MaterialRemoveRequest &request){
            m_remove_mutex.lock();
            m_set_mutex.lock();
            if (!m_set_queue.remove_request(request.entity)) {
                m_add_mutex.lock();
                if (!m_add_queue.remove_request(request.entity)) {
                    m_remove_queue.push_back(request.entity);
                }
                m_add_mutex.unlock();
            }
            else {
                m_remove_queue.push_back(request.entity);
            }
            m_set_mutex.unlock();
            m_remove_mutex.unlock();
        }

        std::vector<entt::entity>& get_remove_queue(){
            m_remove_mutex.lock();
            return m_remove_queue;
        }

        // uint32_t get_remove_request_count(){return m_remove_requests.m_requests.size();}

        void clear_remove_queue(){
            m_remove_queue.clear();
            m_remove_mutex.unlock();
        }

    private:
        RequestQueue<T> m_add_queue;
        RequestQueue<T> m_set_queue;
        std::vector<entt::entity> m_remove_queue;
        //RequestQueue<T> m_remove_queue;

        std::mutex m_set_mutex;
        std::mutex m_add_mutex;
        std::mutex m_remove_mutex;
    };
}
