#pragma once

namespace Syris{
    template <typename FuncSignature>
    struct FastFunction;

    template<typename ReturnType, typename... Args>
    struct FastFunction<ReturnType(Args...)>{
        FastFunction(ReturnType(*function_ptr)(Args...)) :m_func(function_ptr) {}
        
        ReturnType operator()(Args... args){
            return m_func(args...);
        }
        //ReturnType(*function_ptr)(Args...) m_func;
        ReturnType(*m_func)(Args...);
    };
}