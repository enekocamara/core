#pragma once
#include <functional>
#include <vector>
#include <optional>
#include <mutex>

namespace Sandbox{
    //a list of functions that can be filled from different threads
    //safely to be executed in another. The functions can be consumed after
    //x calls or left to update forever or until pop is called
    //each function will have a unique id per AsyncToSyncQueue instance
    class AsyncToSyncQueue{
    public:
        using FuncID = uint32_t;
        struct AsyncFunction
        {
            std::function<void(void)> function;
            std::optional<uint32_t> calls_to_be_consumed;
        };
        FuncID add(AsyncFunction &func);
        void pop(FuncID id);
        void exec_all();

    private:
        std::unordered_map<FuncID, AsyncFunction> m_functions;
        std::mutex m_mutex;
        FuncID m_current_id = 1;
    };
}