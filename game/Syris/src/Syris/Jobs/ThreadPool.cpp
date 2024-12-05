#include "ThreadPool.hpp"
#include <functional>
#include <chrono>
#include "Syris/log/Log.h"
#include <iostream>

namespace Syris{
    ThreadPool::ThreadPool(uint32_t thread_count){
        for (uint32_t i = 0; i < thread_count; i++){
            m_threads.emplace_back(std::bind(&ThreadPool::main_thread_loop, this, std::placeholders::_1), i);    
        }
    }

    void ThreadPool::main_thread_loop(uint32_t id){
        while(true){
            Job task;
            {
                std::unique_lock<std::mutex> unique_mutex(m_jobs_mutex);
                m_cond_var.wait(unique_mutex, [this]{ return m_stop || !m_jobs.empty();});
                if (m_stop && m_jobs.empty())
                    return;
                task = m_jobs.front();
                m_jobs.pop();
            }
            task();
        }
    }
}