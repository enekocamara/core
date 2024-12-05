#pragma once
#include <vector>
#include <tuple>

#include "Syris/utils/std/meta.h"

namespace Syris{

    template<typename...Args>
    class VectorStorage{
        private:

            template<std::size_t Index>
            using Type = typename std::tuple_element<Index, std::tuple<Args...>>::type;

            template<typename T>
            requires(meta::TypeIsInTypes<T, Args...>)
            static constexpr size_t get_type_index(){
               return meta::type_index<T, Args...>();
            }

        public:
            template<typename T>
            requires(meta::TypeIsInTypes<T, Args...>)
            T get(std::size_t index) {
                return get_storage<T>()[index];
            }
            
            template<typename T>
            requires(meta::TypeIsInTypes<T, Args...>)
            void push(T&& val){
                get_storage<T>().push_back(val);
            }

            template<typename T>
            requires(meta::TypeIsInTypes<T, Args...>)
            std::vector<T>& get_storage(){
                return std::get<get_type_index<T>()>(m_storage);
            }
        private:
            
            std::tuple<std::vector<Args>...> m_storage;
    };
}