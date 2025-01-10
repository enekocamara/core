#pragma once

#include "DynamicLibraryLoader.hpp"
#include "Syris/utils/filesystem/FileWatcher.hpp"

namespace Syris{
    /*
    class DLL_Watcher{
        public:
            DLL_Watcher() = delete;
            DLL_Watcher(const DLL_Watcher& ref) = delete;
            DLL_Watcher& operator=(const DLL_Watcher& ref) = delete;

            DLL_Watcher(std::string path);
            void check();
            DynamicLibraryLoader& get_dll(){return m_dll;}
        private:
            FileWatcher m_file_watcher;
            DynamicLibraryLoader m_dll;
    };
    */
}
