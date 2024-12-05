#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <expected>

namespace Syris{
    class FileWatcher{
        public:
            enum class Event{
                UpToDate,
                Modified,
                NotFound
            };
            enum class CurrentState{
                UpToDate,
                OutOfDate,
                FileNotFound
            };
            FileWatcher() = delete;
            FileWatcher(const FileWatcher& ref) = delete;
            FileWatcher& operator=(const FileWatcher& ref) = delete;
            //FileWatcher(WatchFile&& ref) = default;
            FileWatcher(std::string path);

            //will update last change
            Event check_file();
            std::expected<std::string, std::string> read_file()const;
            void set_up_to_date();
            //void set_status(CurrentState state){m_state = state;}


            const std::string& get_path()const{return m_path;}
            std::filesystem::file_time_type get_last_change()const{return m_last_change;}
            //change file path

        private:
            std::filesystem::file_time_type m_last_change;
            //std::string m_file_source;
            std::string m_path;
            CurrentState m_state;
    };
}