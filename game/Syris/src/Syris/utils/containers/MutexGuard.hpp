#pragma once
#include <mutex>

namespace Syris{

    template<typename T>
    class MutexGuard{
        public:
            template<typename... Args>
            explicit MutexGuard(Args...args):m_object(std::forward<Args>(args)...){}
            explicit MutexGuard(){}
            MutexGuard(MutexGuard &&other) noexcept
                : m_object(std::move(other.m_object)) { /* Move other members as needed */ }
            std::pair<T&, std::unique_lock<std::mutex>> get(){
                return {m_object, std::unique_lock(m_mutex)};
            }
            T& get_unguarded(){
                return {m_object};
            }
            T& lock(){m_mutex.lock();return m_object;}
            void unlock(){m_mutex.unlock();}
        private:
            std::mutex m_mutex;
            T m_object;
    };
    template<typename T, typename...Args>
    inline MutexGuard<T> make_guarded(Args... args){
        return MutexGuard<T>(std::forward<Args>(args)...);
    }
}