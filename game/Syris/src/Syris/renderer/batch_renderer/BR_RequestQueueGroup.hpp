#pragma once
#include "BR_DynRequestQueue.hpp"
#include "Syris/utils/std/meta.h"
#include "Syris/utils/Debug.h"
namespace Syris{

    struct RequestQueues {
        std::vector<DynQueue> queues;
        std::size_t request_count;
    };

    template<typename T>
    concept RequestConcept = std::is_same_v<T, BR_Request>;
    template<typename T>
    concept RequestSparseConcept = std::is_same_v<T, BR_RequestSparse>;

    template<typename T>
    concept IsRequest = std::is_same_v<T, BR_Request> || std::is_same_v<T, BR_RequestSparse>;

    template<IsRequest RequestType, typename...Types>
    class BR_RequestQueueGroup{
    public:
        BR_RequestQueueGroup(std::vector<std::size_t> sizes){
            std::cout << "queue group size: " << sizes.size() << '\n';
            for (auto size : sizes) {
                m_queues.emplace_back(size);
                //m_requests_per_queue.push_back(0);
            }
        }
        BR_RequestQueueGroup(const BR_RequestQueueGroup &ref) = delete;
        BR_RequestQueueGroup& operator=(const BR_RequestQueueGroup &ref) = delete;

        void add_request(BR_Request& request) requires RequestConcept<BR_Request>{
            std::tuple<Types...>& data = *static_cast<std::tuple<Types...>*>(request.data);
            meta::for_each_tuple(data, [this, entity = request.entity]<std::size_t Index, typename T>(T& value){
                BR_Request queue_request{
                    .entity = entity,
                    .data = &value
                };
                m_queues[Index].add_request(queue_request);

            });/*
            for (auto [index, data] : request.data){
                BR_Request queue_request{
                    .entity = request.entity,
                    .data = data
                };
                m_queues[index].add_request(queue_request);
            }*/
            m_requests_count++;
        }
        void add_request(BR_RequestSparse& request) requires RequestConcept<BR_Request>
        {
            ASSERT(request.data.size() == m_queues.size(), "Data for all queues must be sent");
            std::set<std::size_t> indices;
            for (auto [index, data] : request.data){
                BR_Request queue_request{
                    .entity = request.entity,
                    .data = data
                };
                m_queues[index].add_request(queue_request);
                indices.insert(index);
            }
            ASSERT(indices.size() == m_queues.size(), "Repeated queue in request data");
            m_requests_count++;
        }
           
        std::vector<std::pair<std::size_t, void *>> set_if_find(BR_RequestSparse& request){
            std::vector<std::pair<std::size_t, void*>> unset;
            for (auto [index, data] : request.data){
                BR_Request queue_request{
                    .entity = request.entity,
                    .data = data
                };
                if (!m_queues[index].set_if_find(queue_request))
                    unset.emplace_back(index, data);
            }
            return unset;
        }

        RequestQueues get_queues(){
            std::vector<DynQueue> queues;
            for (auto& queue : m_queues){
                queues.push_back(queue.get_queue());
            }
            return { queues, m_requests_count};
        };

        bool remove_request(entt::entity entity)requires RequestConcept<BR_Request>{
            bool all_removed = false;
            for (auto& queue : m_queues){
                bool removed = queue.remove_request(entity);
                if (all_removed == true && removed == false)
                    BREAK_POINT("All queues must have one request");
                if (removed == false)
                    return false;
                all_removed = removed;
            }
            return all_removed;
        }
        bool remove_request(entt::entity entity) requires RequestConcept<BR_RequestSparse>{
            bool any_removed = false;
            for (auto& queue : m_queues){
                bool removed = queue.remove_request(entity);
                if (removed)
                    any_removed = true;
            }
            return any_removed;
        }

        void clear(){
            for (auto& queue : m_queues){
                queue.clear();
            }
            m_requests_count = 0;
        }
        private:
            std::vector<DynRequestQueue> m_queues;
            std::size_t m_requests_count = 0;
            std::vector<std::size_t> m_sizes;
    };
}