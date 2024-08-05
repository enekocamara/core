#pragma once
#include <functional>

namespace core{

    enum class JobPriority{
        Main,
        Secondary
    };

    template <typename ReturnValue, typename FunctionParamerter>
    class Job{
        public:
            struct CreateInfo{
                std::function<ReturnValue(FunctionParamerter)>entrypoint; 
                std::function<ReturnValue(FunctionParamerter)>handle;
            };
            Job(CreateInfo& info);
            Job()=delete;
            Job(const Job& ref) = delete;
            Job& operator=(const Job& ref) = delete;
        private:
            std::function<ReturnValue(FunctionParamerter)>entrypoint; 
            std::function<ReturnValue(FunctionParamerter)>handle; 
    };
}
