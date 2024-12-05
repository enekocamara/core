#pragma once
#include "Syris/utils/FastFunction.hpp"

namespace Syris{
    //code from  https://stackoverflow.com/questions/15249465/c-dynamically-load-arbitrary-function-from-dll-into-stdfunction
    template <typename T>
    struct TypeParser
    {
    };

    template <typename Ret, typename... Args>
    struct TypeParser<Ret(Args...)>
    {
        using return_type = Ret;
        static std::function<Ret(Args...)> createFunction(const FARPROC lpfnGetProcessID)
        {
            return std::function<Ret(Args...)>(reinterpret_cast<Ret(__stdcall *)(Args...)>(lpfnGetProcessID));
        }
        static FastFunction<Ret(Args...)> create_fast_function(const FARPROC lpfnGetProcessID)
        {
            return FastFunction<Ret(Args...)>(reinterpret_cast<Ret(__stdcall *)(Args...)>(lpfnGetProcessID));
        }
    };


}