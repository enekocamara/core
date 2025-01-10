#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <expected>
#include <unordered_map>
#include <Syris/scripts/DynamicLibraryLoader.hpp>
#include "Syris/utils/containers/MutexGuard.hpp"

namespace Syris{
    class DLL_Watcher{
        public:
        struct Item{
            enum class Type{
                File,
                Folder
            };
            enum class Status{
                UpToDate,
                Modified,
                Deleted
            };
            Type type;
            Status status;
        };

        enum class Status{
            FolderNotFound,
            Nominal
        };

        DLL_Watcher(std::string path_dir, std::string dll_root_name);

        DLL_Watcher() = delete;
        DLL_Watcher(const DLL_Watcher &ref) = delete;
        DLL_Watcher &operator=(const DLL_Watcher &ref) = delete;
        DynamicLibraryLoader& get_dll() {return m_dll;}
        void check();
        private:
            std::string m_path_dir;
            std::string m_dll_root_name;
            std::string m_current_dll_name;
            uint64_t m_current_id = 0;
            DynamicLibraryLoader m_dll;
            Status m_status;
    };
}