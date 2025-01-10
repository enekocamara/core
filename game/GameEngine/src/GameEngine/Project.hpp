#pragma once
#include <string>
#include "Syris/utils/filesystem/DLL_Watcher.hpp"

namespace GameEngine{
    class Project{
        public:
            struct CreateInfo{

            };
            Project() = delete;
            Project(const Project& ref) = delete;
            Project& operator=(const Project& ref) = delete;
            Project(CreateInfo info);

        private:
            std::string m_path_to_project_root;
            Syris::DLL_Watcher m_dll_watcher;
    };
}