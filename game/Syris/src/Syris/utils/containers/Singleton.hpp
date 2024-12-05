#pragma once
#include "Syris/utils/Breakpoint.h"
#include "MutexGuard.hpp"

namespace Syris {
    template<class T>
    class Singleton {
    public:
        static T& get() {
            return holder();
        }

        template<typename T, typename... Args>
        static T& init(Args&&... args) {
            static bool init = false;
            if (init)
                BREAK_POINT("Already initialized");
            init = true;
            return holder(std::make_unique<T>(std::forward<Args>(args)...));
        }
    private:
        static T& holder(std::unique_ptr<T> val = nullptr) {
            static Singleton<T> holder(std::move(val));
            return *holder.m_object;
        }

        Singleton(std::unique_ptr<T> ptr) : m_object(std::move(ptr)) {}
        std::unique_ptr<T> m_object;
    };

    template<class T>
    class MutexGuardedSingleton {
    public:
        static T& get() {
            return holder();
        }

        template<typename T, typename... Args>
        static T& init(Args&&... args) {
            static bool init = false;
            if (init)
                BREAK_POINT("Already initialized");
            init = true;
            return holder(std::make_unique<T>(std::forward<Args>(args)...));
        }
    private:
        static T& holder(std::unique_ptr<T> val = nullptr) {
            static MutexGuardedSingleton<T> holder(std::move(val));
            return *holder.m_object.get();
        }

        MutexGuardedSingleton(std::unique_ptr<T> ptr) : m_object(std::move(ptr)) {}

        MutexGuard<std::unique_ptr<T>> m_object;
    };
}