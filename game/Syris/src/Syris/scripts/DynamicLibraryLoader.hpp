#pragma once
#include <mutex>
#include <functional>
#include <expected>

#include "Syris/include/windows.h"
#include "Syris/log/Log.h"
#include "Syris/utils/Breakpoint.h"
#include "Syris/utils/std/meta.h"
#include "TypeParser.h"
#include "Syris/utils/FastFunction.hpp"

namespace Syris{
    class DynamicLibraryLoader{
        public:
            struct CreateInfo{
                std::string path;
            };
            DynamicLibraryLoader() = delete;
            DynamicLibraryLoader(const DynamicLibraryLoader& ref) = delete;
            DynamicLibraryLoader& operator=(const DynamicLibraryLoader& ref) = delete;
            
            DynamicLibraryLoader(CreateInfo info):m_path(info.path){
                m_dll_handle = LoadLibrary(info.path.c_str());
                if (!m_dll_handle){
                    CORE_ERROR(std::format("Failed to load DLL: [{}]", info.path));
                    throw std::runtime_error("Failed to create DLL");
                }
                // Load the function
            }
            ~DynamicLibraryLoader(){
                if (m_dll_handle){
                    FreeLibrary(m_dll_handle);
                }
            }
            template<typename T>
            std::expected<std::function<T>, std::string> get_function(std::string name){
                std::unique_lock<std::mutex> lock(m_function_mutex);
                auto it = m_functions.find(name);
                if (it != m_functions.end()){
                    return TypeParser<T>::createFunction(it->second);
                }else{
                    FARPROC lpfnGetProcessID = GetProcAddress(m_dll_handle, name.c_str());
                    if (!lpfnGetProcessID){
                        CORE_ERROR(std::format("Failed to load function: {}", name));
                        return std::unexpected(std::format("Failed to load functin: {}", name));
                    }
                    m_functions[name] = lpfnGetProcessID;
                    return TypeParser<T>::createFunction(*lpfnGetProcessID);
                }
            }

            template<typename T>
            std::expected<FastFunction<T>, std::string> get_function_ptr(std::string name){
                std::unique_lock<std::mutex> lock(m_function_mutex);
                auto it = m_functions.find(name);
                if (it != m_functions.end()){
                    return TypeParser<T>::create_fast_function(it->second);
                }else{
                    FARPROC lpfnGetProcessID = GetProcAddress(m_dll_handle, name.c_str());
                    if (!lpfnGetProcessID){
                        CORE_ERROR(std::format("Failed to load function: {}", name));
                        return std::unexpected(std::format("Failed to load functin: {}", name));
                    }
                    m_functions[name] = lpfnGetProcessID;
                    return TypeParser<T>::create_fast_function(*lpfnGetProcessID);
                    //return TypeParser<T>::createFunction(*lpfnGetProcessID);
                }
            }

            void reload(std::string new_path){
                std::unique_lock<std::mutex> lock(m_reload_mutex);
                m_path = new_path;
                if (m_dll_handle){
                    FreeLibrary(m_dll_handle);
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                m_dll_handle = LoadLibrary(m_path.c_str());
                if (!m_dll_handle){
                    DWORD error_code = GetLastError();
                    std::stringstream ss;
                    ss << error_code;
                    std::string error_message = std::format("Failed to load DLL: [{}] Error code: [{}]", m_path, ss.str());
                    BREAK_POINT(error_message);
                }
                m_functions.clear();
                m_version++;
                CORE_INFO("DLL reloaded");
            }
            uint64_t get_version()const {return m_version;}
            std::unique_lock<std::mutex> block_reloading(){return std::unique_lock(m_reload_mutex);}
        private:
            std::string m_path;
            HMODULE m_dll_handle = nullptr;// Handle to the loaded DLL
            std::unordered_map<std::string, FARPROC> m_functions;
            uint64_t m_version = 0;
            std::mutex m_reload_mutex;
            std::mutex m_function_mutex;
    };
}