#include "DLL_Watcher.hpp"
#include <iostream>
#include <string>
#include <charconv>
#include "Syris/utils/Debug.h"
namespace Syris{

    std::expected<uint64_t, std::string> get_id(const std::string& name, const std::string& dll_root_name){
        std::size_t id = 0;
        std::string number = name.substr(dll_root_name.size() + 1, 6); 
        number += name.substr(dll_root_name.size() + 10, 6); 
        auto res = std::from_chars(number.data(), number.data() + number.size(), id); // 4 is for .dll
        if (res.ec == std::errc())
            return id;
        else
            return std::unexpected("cant be void for error");

    }

    std::string get_latest_dll(const std::string& path_dir, const std::string& dll_root_name){
        uint64_t current_id = 0;
        std::string current_dll_name;
        for (const auto &entry : std::filesystem::directory_iterator(path_dir)){
            std::string name(entry.path().filename().string());
            if (name.ends_with(".dll")){
                uint64_t id = get_id(name, dll_root_name).value();
                if (current_id < id){
                    current_id = id;
                    current_dll_name = name;
                }
            }
        }
        if (current_id == 0)
            BREAK_POINT("Coundn't find dll");
        return current_dll_name;
    }


    DLL_Watcher::DLL_Watcher(std::string path_dir, std::string dll_root_name)
        :  m_path_dir(path_dir),
           m_dll_root_name(dll_root_name),
           m_current_dll_name(get_latest_dll(m_path_dir, m_dll_root_name)),
           m_current_id(get_id(m_current_dll_name, m_dll_root_name).value()),
           m_dll(DynamicLibraryLoader::CreateInfo{path_dir + '\\' + m_current_dll_name}){
        
    }

    void DLL_Watcher::check(){
        std::string latest = get_latest_dll(m_path_dir, m_dll_root_name);
        uint64_t id = get_id(latest, m_dll_root_name).value();
        if (id > m_current_id){
            m_current_id = id;
            m_current_dll_name = latest;
            m_dll.reload(m_path_dir + '\\' + m_current_dll_name);
        }
    }
}