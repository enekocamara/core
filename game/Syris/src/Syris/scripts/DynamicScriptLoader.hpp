#pragma once
#include <functional>
#include "Syris/include/windows.h"

#include "Syris/log/Log.h"
#include "Syris/utils/Breakpoint.h"
#include "TypeParser.h"
//#define DLSCRIPT extern "C" __declspec(dllexport)

namespace Syris{
    

    //end code

    template<typename F>
    class DynamicScriptLoader{
        public:
            struct CreateInfo{
                std::string path;
                std::string function_name;
            };
            DynamicScriptLoader(CreateInfo info){
                m_dll_handle = LoadLibrary(info.path.c_str());
                if (!m_dll_handle){
                    CORE_ERROR(std::format("Failed to load DLL: {}", info.path));
                    throw std::runtime_error("Failed to load DLL");
                }
                // Load the function
                FARPROC lpfnGetProcessID = GetProcAddress(m_dll_handle, info.function_name.c_str());
                if (!lpfnGetProcessID)
                {
                    CORE_ERROR(std::format("Failed to load function: ", info.function_name));
                    FreeLibrary(m_dll_handle);
                    throw std::runtime_error(std::format("Failed to load function: {}", info.function_name));
                }
                m_func_ptr = TypeParser<F>::createFunction(lpfnGetProcessID);
            }
            ~DynamicScriptLoader(){
                if (m_dll_handle){
                    FreeLibrary(m_dll_handle);
                }
            }
            template<typename... Args>
            auto operator()(Args&&... args) -> std::invoke_result_t<F, Args...>{
                return std::invoke(m_func_ptr, std::forward<Args>(args)...);
            }
        private:
            HMODULE m_dll_handle = nullptr;// Handle to the loaded DLL
            std::function<F> m_func_ptr;// Function pointer of type F
    };
}