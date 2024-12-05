#include "DLL_Watcher.hpp"

namespace Syris{
    DLL_Watcher::DLL_Watcher(std::string path):m_dll({path}),m_file_watcher(path){

    }
    void DLL_Watcher::check(){
        FileWatcher::Event event = m_file_watcher.check_file();
        if (event == FileWatcher::Event::Modified){
            std::cout << "Reloading dll\n";
            m_dll.reload();
            m_file_watcher.set_up_to_date();
        }else if (event == FileWatcher::Event::UpToDate)
            return;
        else if (event == FileWatcher::Event::NotFound)
            BREAK_POINT("Failed to find dll");
        else
            BREAK_POINT("Shouldn't be another event");
    }
}