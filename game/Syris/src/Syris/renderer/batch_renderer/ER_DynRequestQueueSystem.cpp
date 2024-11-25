#include "ER_DynRequestQueueSystem.hpp"

/*
namespace Syris{
       BR_DynRequestQueueSystem::ER_DynRequestQueueSystem(uint64_t type_size)
            : m_add_queue(type_size),
              m_set_queue(type_size)
            {
                //todo
                throw std::runtime_error("TODO er dynamic request queue system");
                exit(1);
                uint64_t attributes_size = 0;/*
                for (auto& [name, type] : attributes.get_vars()){
                    attributes_size += dyn_sizeof_type(type);
                }
                m_attribute_set_requests.resize(attributes_size);
            // not ready
            //for (auto& [name, type] : attributes.get_vars()){
            //    m_attribute_set_requests.emplace_back(strdup(name.c_str()), type, new std::byte[dyn_sizeof_type(type)]);
            //}
        }
       BR_DynRequestQueueSystem::~ER_DynRequestQueueSystem(){
            //not ready
            //for (auto& attribute : m_attribute_set_requests){
            //    delete[] attribute.data;
            //   free(attribute.name);
            //}
        }

        voidBR_DynRequestQueueSystem::add_entity(BR_DynAddRequest &request)
        {
            m_add_mutex.lock();
           BR_DynPlainRequest typed_request{ request.entity, request.data};
            m_add_queue.add_request(typed_request);
            m_add_mutex.unlock();
        }

        
        DynQueueBR_DynRequestQueueSystem::get_add_queue(){
            m_add_mutex.lock();
            return m_add_queue.get_queue();
        }

        voidBR_DynRequestQueueSystem::clear_add_queue(){
            m_add_queue.clear();
            m_add_mutex.unlock();
        }

        
        voidBR_DynRequestQueueSystem::set_entity(BR_DynSetRequest &request)
        {
            m_set_mutex.lock();
            m_add_mutex.lock();
           BR_DynPlainRequest typed_request{ request.entity, request.data};
            if (!m_add_queue.set_if_find(typed_request))
            {
                m_set_queue.add_request(typed_request);
            }
            m_add_mutex.unlock();
            m_set_mutex.unlock();
        }

      
        DynQueueBR_DynRequestQueueSystem::get_set_queue()
        {
            m_set_mutex.lock();
            return m_set_queue.get_queue();
        }

        // uint32_t get_set_request_count() { return m_set_queue.m_requests.size(); }

        voidBR_DynRequestQueueSystem::clear_set_queue()
        {

            m_set_queue.clear();
            m_set_mutex.unlock();
        }

        voidBR_DynRequestQueueSystem::remove_entity(BR_DynRemoveRequest &request){
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

        std::vector<entt::entity>&BR_DynRequestQueueSystem::get_remove_queue(){
            m_remove_mutex.lock();
            return m_remove_queue;
        }

 

        void BR_DynRequestQueueSystem::clear_remove_queue(){
            m_remove_queue.clear();
            m_remove_mutex.unlock();
        }

        void BR_DynRequestQueueSystem::set_attribute(BR_DynSetAttributeRequest& request){
            if (request.size != m_set_attribute_request.size())
                throw std::runtime_error("request size doens't match m_set_attribute_request size.");
            memcpy(m_set_attribute_request.data(), request.data, request.size);
            // not ready for this
            //for (auto& var : request.data){
            //   bool set = false;
            //    for (auto& attribute : m_attribute_set_requests){
            //        if (strcmp(attribute.name, var.name) == 0){
            //            memcpy(attribute.data, var.data, dyn_sizeof_type(var.type));
            //            set = true;
            //        }
            //    }
            //    if (!set)
            //        throw std::runtime_error("request variable not in attribute list");
            //}
            
        }
}*/