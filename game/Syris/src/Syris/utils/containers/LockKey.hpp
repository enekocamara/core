#pragma once

#include <mutex>

namespace Syris{
    class LockKey{
        public:
            LockKey& lock();
        private:
            uint64_t m_key = 0;
            std::mutex m_mutex;
    };
}