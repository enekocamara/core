#pragma once

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
                    CORE_ERROR(std::format("Failed to load DLL: {}", info.path));
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

            void reload(){
                if (m_dll_handle){
                    FreeLibrary(m_dll_handle);
                }
                m_dll_handle = LoadLibrary(m_path.c_str());
                if (!m_dll_handle){
                    CORE_ERROR(std::format("Failed to load DLL: {}", m_path));
                    throw std::runtime_error("Failed to create DLL");
                }
                m_functions.clear();
            }
        private:
            std::string m_path;
            HMODULE m_dll_handle = nullptr;// Handle to the loaded DLL
            std::unordered_map<std::string, FARPROC> m_functions;
    };
}