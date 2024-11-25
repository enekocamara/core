//#include "BR_RequestQueueSystem.hpp"

namespace Syris{
    
    /***************** MATERIAL QUEUE SYSTEM IMPLEMENTATION *************/
/*
    template<typename T>
    BR_RequestQueueSystem<T>::BR_RequestQueueSystem(){}
    template<typename T>
    BR_RequestQueueSystem<T>::~BR_RequestQueueSystem(){}
*/
    /***** ADD *****/
/*
    template<typename T>
    void BR_RequestQueueSystem<T>::add_entity(MaterialAddRequest& request){
        m_add_mutex.lock();
        m_add_queue.add_request(request);
        m_add_mutex.unlock();
    }

    template<typename T>
    Queue<T> BR_RequestQueueSystem<T>::get_add_queue(){
        m_add_mutex.lock();
        return m_add_queue.get_queue(); 
    }

    template<typename T>
    void BR_RequestQueueSystem<T>::clear_add_queue(){
        m_add_queue.clear();
        m_add_mutex.unlock();
    }
*/

                        /***** SET ******/
/*
    template<typename T>
    void BR_RequestQueueSystem<T>::set_entity(MaterialSetRequest &request){
        m_set_mutex.lock();
        m_add_mutex.lock();
        if (!m_add_queue.set_if_find(request)){
            m_set_queue.add_request(request);
        }
        m_add_mutex.unlock();
        m_set_mutex.unlock();
    }

    template<typename T>
    Queue<T> BR_RequestQueueSystem<T>::get_set_queue(){
        m_set_mutex.lock();
        return m_set_queue.get_queue(); 
    }

    template<typename T>
    void BR_RequestQueueSystem<T>::clear_set_queue(){
        m_set_queue.clear();
        m_set_mutex.unlock();
    }
*/
    /***** REMOVE *****/
/*
    template<typename T>
    void BR_RequestQueueSystem<T>::remove_entity(MaterialRemoveRequest& request){
        m_remove_mutex.lock();
        m_set_mutex.lock();
        if (!m_set_queue.remove_request(request.entity)){
            m_add_mutex.lock();
            if (!m_add_queue.remove_request()){
                m_remove_queue.add_request(request);
            }
            m_add_mutex.unlock();
        }
        m_set_mutex.unlock();
        m_remove_mutex.unlock();
    }

    template<typename T>
    Queue<T> BR_RequestQueueSystem<T>::get_remove_queue(){
        m_remove_mutex.lock();
        return m_remove_queue.get_queue(); 
    }

    template<typename T>
    void BR_RequestQueueSystem<T>::clear_remove_queue(){
        m_remove_queue.clear();
        m_remove_mutex.unlock();
    }
    */
}