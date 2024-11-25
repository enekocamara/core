#pragma once
#include <functional>
#include <mutex>
#include <unordered_map>
#include <set>

#include <entt.hpp>
#include "ER_DynRequests.h"
#include "ER_DynRequestQueue.hpp"
#include "Syris/shader/Layout.hpp"

/*/
namespace Syris{
    */
    /** @brief Manages requests in a thread safe way by pooling request */
    /*
    class BR_DynRequestQueueSystem
    {
    public:
        BR_DynRequestQueueSystem(uint64_t type_size);
        ~BR_DynRequestQueueSystem();

        BR_DynRequestQueueSystem(const BR_DynRequestQueueSystem&ref) = delete;
        BR_DynRequestQueueSystem(BR_DynRequestQueueSystem&ref) = delete;
        BR_DynRequestQueueSystem&operator=(const BR_DynRequestQueueSystem&ref) = delete;

        void add_entity(BR_DynAddRequest &request);
        */
        /**
         * @brief Combines the append request that go to the same subbufer to reduce calls
            its size is not representative of the number of append requests done.
            It will lock the append queue until clear_append_queue is called.
           @return An iterable struct with all the append requests in the queue
        *//*
        DynQueue get_add_queue();
       
       */
        /** @brief Clear all the data related to append requests and unlock the append queue*/
        //void clear_add_queue();
        

        /**
         * @brief Stores the set request in the set queue, all memory being stored in a single
            memqueue
        */
        //void set_entity(BR_DynSetRequest &request);

        /**
         * @return Returns a iterable struct holding all the set request. It will lock the set queue
         * until clear_set_queue is called
         */
        /*
        DynQueue get_set_queue();

        void clear_set_queue();

        void remove_entity(BR_DynRemoveRequest &request);
        std::vector<entt::entity>& get_remove_queue();
        void clear_remove_queue();

        void set_attribute(BR_DynSetAttributeRequest& request);
    private:
        DynRequestQueue m_add_queue;
        DynRequestQueue m_set_queue;
        std::vector<entt::entity> m_remove_queue;
        std::vector<std::byte> m_set_attribute_request;
        //RequestQueue<T> m_remove_queue;

        std::mutex m_set_mutex;
        std::mutex m_add_mutex;
        std::mutex m_remove_mutex;
    };
}*/
