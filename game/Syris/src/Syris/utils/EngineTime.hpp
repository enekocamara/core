#pragma once
#include <chrono>

namespace Syris::engine_time{
    using hrc = std::chrono::high_resolution_clock;
    class Time{
    public:
        Time();
        ~Time() {};
        void start();
        void next_frame();
        float get_delta_ms()const {return m_delta_time_ms;}
        float get_running_time_ms()const {return m_running_ms;}
        hrc::time_point get_frame_start()const {return m_this_frame_start;}

    private:
        hrc::time_point m_init_time;
        hrc::time_point m_last_frame;
        hrc::time_point m_this_frame_start;
        float m_running_ms;
        float m_delta_time_ms;
    };


    class FPSOverlay{
    public:
        FPSOverlay(std::string name);
        ~FPSOverlay();
        void next_frame(const Time& time);
        void render_frame_count();

    private:
        int m_fps = 0;
        int m_frame_count = 0;
        hrc::time_point m_last_check; 
        std::chrono::duration<float, std::milli> m_interval = std::chrono::seconds(1);
        bool first = true;
        std::string m_name;
    };

    class FPS{
    public:
        struct CreateInfo{
            std::string name;
        };
        FPS(CreateInfo info);
        FPS() = delete;
        ~FPS();
        void start();
        void next_frame();
        void render_frame_count();
        const Time& get_time(){return m_time;}
    private:
        Time m_time;
        int m_fps = 0;
        int m_frame_count = 0;
        hrc::time_point m_last_check; 
        std::chrono::duration<float, std::milli> m_interval = std::chrono::seconds(1);
        std::string m_name;
    };
}
