#include "FileWatcher.hpp"
#include "Syris/log/Log.h"
#include "Syris/utils/Debug.h"
#include "Syris/utils/filesystem/readfile.h"

namespace Syris{
    FileWatcher::FileWatcher(std::string path):m_path(path){
        m_last_change = std::filesystem::last_write_time(m_path);
        //Event event = check_file();
    }

    // will update last change
    FileWatcher::Event FileWatcher::check_file(){
        try{
            std::filesystem::file_time_type current_last_changed = std::filesystem::last_write_time(m_path);
            if (current_last_changed == m_last_change){
                m_state = CurrentState::UpToDate;
                return Event::UpToDate;
            }
            m_state = CurrentState::OutOfDate;
            return Event::Modified;
        }catch (std::exception& e){
            CORE_INFO(std::format("Couldn't get last write time.\nPath: '{}'", m_path).c_str());
            m_state = CurrentState::FileNotFound;
            return Event::NotFound; 
        }
    }
    std::expected<std::string, std::string> FileWatcher::read_file()const{
        auto res = utils::read_file(m_path.c_str());
        return res;
    }
    void FileWatcher::set_up_to_date(){
        m_state = CurrentState::UpToDate;
        try{
            m_last_change = std::filesystem::last_write_time(m_path);
        }catch (std::exception& e){
            BREAK_POINT("Should never happen");
        }
    }
}