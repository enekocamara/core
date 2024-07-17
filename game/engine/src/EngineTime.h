#pragma once
#include <chrono>

namespace engine_time{
    typedef std::chrono::high_resolution_clock::time_point  ge_time_point;
    struct Time{
        ge_time_point init_time;
        float running_ms;
        ge_time_point last_frame;
        float delta_time_ms;
    };

}
