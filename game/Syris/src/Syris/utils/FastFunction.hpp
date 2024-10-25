#pragma once

namespace Syris{

    template<typename ReturnType, typename... Args>
    struct FastFunction{
        FastFunction(ReturnType(*function_ptr)(Args...)) :m_func(function_ptr) {}
        
        ReturnType operator()(Args... args){
            return m_func(args...);
        }
        //ReturnType(*function_ptr)(Args...) m_func;
        ReturnType(*m_func)(Args...);
    };
}