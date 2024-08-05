#pragma once
#include <string>
#include <unordered_map>
#include "Job.hpp"
namespace core {
    /*
     * This singleton class will be the manager of the entire application
     * and its start point wether for just server, actual gaming client
     * or development with tools.
     *
     * It control resourcess such as threads ports, and organizes the application.
     * It handles asyncronous code
     *
     *
     *
     * */
    class Director{
        public:
            using JobId = uint32_t;
            struct CreateInfo{
                std::string name;
                uint8_t max_thread_count;
            };

            Director() = delete;
            Director(CreateInfo* info);
            Director(const Director& ref) = delete;
            Director& operator=(const Director& ref) = delete;

            /*
             * initialises the director
            */
            static Director& init(CreateInfo &info){
                return holder(&info);
            }

            static Director& get(){
                return holder();
            }

            template<typename ReturnType, typename InputType>
            void attach_to_thread(Job<ReturnType, InputType>& job);

            template <typename ReturnValue, typename FunctionParamerter>
            uint32_t create_job(Job<ReturnValue, FunctionParamerter>::CreateInfo& info){
                uint32_t id = next_job_id++; 
                m_jobs[id] = new Job<ReturnValue, FunctionParamerter>(info);
                return id;
            }

            void queueJob(uint32_t id, uint32_t priority);

            template <typename ReturnValue, typename FunctionParameter>
            Job<ReturnValue, FunctionParameter*> get_job(JobId id){
                return reinterpret_cast<Job<ReturnValue, FunctionParameter> *>(m_jobs[id]);
            };

        private:

            static Director& holder(CreateInfo *info = nullptr){
                static Director *director = new Director(info);
                if (info == nullptr && director == nullptr)
                    exit(1);
                return *director;
            }
            uint32_t next_job_id = 0;
            std::unordered_map<uint32_t, void*> m_jobs;
            std::unordered_map<uint32_t, JobPriority> m_jobs_priorities;
    };
}
