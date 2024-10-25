#include "AsyncToSyncQueue.hpp"

namespace Sandbox{

    AsyncToSyncQueue::FuncID AsyncToSyncQueue::add(AsyncFunction& func){
        m_mutex.lock();
        FuncID id = m_current_id++;
        m_functions[id] = func;
        m_mutex.unlock();
        return id;
    }
    void AsyncToSyncQueue::pop(FuncID id){
        m_mutex.lock();
        m_functions.erase(id);
        m_mutex.unlock();
    }
    void AsyncToSyncQueue::exec_all(){
        std::vector<FuncID> ids_to_be_removed;
        m_mutex.lock();
        for (auto& func : m_functions){
            func.second.function();
            if (func.second.calls_to_be_consumed.has_value() && --func.second.calls_to_be_consumed.value() == 0)
                ids_to_be_removed.push_back(func.first);
        }
        for (auto id : ids_to_be_removed){
            m_functions.erase(id);
        }
        m_mutex.unlock();
    }
}