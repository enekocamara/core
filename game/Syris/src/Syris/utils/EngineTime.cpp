#include "EngineTime.hpp"

#include "Syris/include/imgui.h"
#include <format>
namespace Syris::engine_time{

    /************** TIME IMPLEMENTATION ***************/

    Time::Time(){
    }
    void Time::start(){
        m_init_time = hrc::now();
        m_this_frame_start = hrc::now();
    }
    void Time::next_frame(){
        m_last_frame = m_this_frame_start;
        m_this_frame_start = hrc::now();
        m_delta_time_ms =
            std::chrono::duration<float, std::milli>(m_this_frame_start - m_last_frame).count();
    }
    

    /************** FPS IMPLEMENTATION ***************/

    FPS::FPS(CreateInfo info):m_name(info.name){

    }
    FPS::~FPS(){

    }
    void FPS::start(){
        m_time.start();
        m_last_check = hrc::now();
    }
    void FPS::next_frame(){
        m_time.next_frame();
        m_frame_count++;
        if (std::chrono::duration<float, std::milli>(m_time.get_frame_start() - m_last_check) > m_interval){
            m_fps = m_frame_count;
            m_frame_count = 0;
            m_last_check = m_time.get_frame_start();
        }
    }
    void FPS::render_frame_count(){
        ImGui::Begin(std::format("{} fps", m_name).c_str());
        ImGui::Text("FPS: %d", m_fps);
        ImGui::End();
    }
    /************** FPS OVERLAY IMPLEMENTATION ***************/

    FPSOverlay::FPSOverlay(std::string name):m_name(name){}
    FPSOverlay::~FPSOverlay(){}
    void FPSOverlay::next_frame(const Time &time){
        if (first){
            m_last_check = time.get_frame_start();
            first = false;
        }
        m_frame_count++;
        if (std::chrono::duration<float, std::milli>(time.get_frame_start() - m_last_check) > m_interval){
            m_fps = m_frame_count;
            m_frame_count = 0;
            m_last_check = time.get_frame_start();
        }

    }
    void FPSOverlay::render_frame_count(){
        ImGui::Begin(std::format("{} fps", m_name).c_str());
        ImGui::Text("FPS: %d", m_fps);
        ImGui::End();
    }
}