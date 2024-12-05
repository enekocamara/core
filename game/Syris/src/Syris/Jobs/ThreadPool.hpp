#pragma once
#include <cinttypes>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <expected>
#include "Syris/log/Log.h"
#include "Syris/utils/Breakpoint.h"

namespace Syris{
    class ThreadPool{
        public:
            using Job = std::function<void()>;
            ThreadPool(uint32_t threads);
            void main_thread_loop(uint32_t id);
            template <typename F, typename... Args>
            auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>{
                using return_type = std::invoke_result_t<F, Args...>;
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );
                std::future<return_type> result = task->get_future();
                {
                    std::unique_lock lock(m_jobs_mutex);
                    if (m_stop){
                        BREAK_POINT("QUEUE CLOSED");
                    }
                    m_jobs.push([task]() {(*task)(); });
                }
                m_cond_var.notify_one(); 
                return result;
            }
        private:
            std::vector<std::thread> m_threads;
            std::queue<Job> m_jobs;
            std::mutex m_jobs_mutex;
            std::condition_variable m_cond_var;
            std::atomic<bool> m_stop = false;
    };
}