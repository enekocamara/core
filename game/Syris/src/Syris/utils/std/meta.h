#pragma once
#include <tuple>
#include <array>
#include "Syris/utils/Assert.h"
//utility for metaprograming

namespace Syris::meta{

    template<typename T, typename...Types>
    concept TypeIsInTypes = (std::is_same_v<T,Types> || ...);

    template<typename T, typename...Types>
    struct type_is_in_types
        : std::disjunction<std::is_same<T, Types>...>{};

 
    template<typename T, typename...Types>
    requires(TypeIsInTypes<T, Types...>)
    constexpr std::size_t type_index(){
        bool is_present[] = {std::is_same_v<T, Types>...};
        for (std::size_t i = 0; i < sizeof...(Types); ++i)
        {
            if (is_present[i])
                return i;
        }
        return -1;//should never reach because of the concept. at least one
                  //is guaranteed to be in there
    }


    template <std::size_t Index, typename... Types>
    constexpr std::size_t size_of_type_in_tuple()
    {
        using Type = typename std::tuple_element<Index, std::tuple<Types...>>::type;
        return sizeof(Type);
    }

    template <typename... Types>
    constexpr auto get_sizes()
    {
        return std::array<std::size_t, sizeof...(Types)>{sizeof(Types)...};
    }

    template <typename... Types>
    std::size_t size_of_type_at(std::size_t index)
    {
        static constexpr auto sizes = get_sizes<Types...>();
        ASSERT(index < sizes.size(), "index out of range");
        return sizes[index];
    }

    // Helper function to apply operation to each tuple element recursively
    template <typename Func, typename Tuple, std::size_t Index>
    auto transform_tuple_impl(Func &&func, Tuple &tuple, std::index_sequence<Index>)
    {
        return std::make_tuple(func(std::get<Index>(tuple)));
    }

    // Recursive case: Applies the function to each tuple element
    template <typename Func, typename Tuple, std::size_t... Indices>
    auto transform_tuple_impl(Func &&func, Tuple &tuple, std::index_sequence<Indices...>)
    {
        return std::make_tuple(func(std::get<Indices>(tuple))...);
    }

    // Function to apply transformation to the entire tuple
    template <typename Func, typename Tuple>
    auto transform_tuple(Func func, Tuple&& tuple)
    {
        return transform_tuple_impl(
            std::forward<Func>(func),
            std::forward<Tuple>(tuple),
            std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }

    template <typename Tuple>
    constexpr uint64_t sizeof_tuple(Tuple& tuple){
        return std::tuple_size<Tuple>();
    }

    template <typename Tuple, uint64_t index>
    struct type_at{
        using type = std::tuple_element_t<index, Tuple>::type;
    };

    template <typename T>
    struct FunctionPointer;
    
    template <typename ReturnType_, typename... Args>
    struct FunctionPointer<ReturnType_(Args...)>
    {
        using PointerType = ReturnType_ (*)(Args...);
        using ReturnType = ReturnType_;
        using Arguments = std::tuple<Args...>;
    };


// Helper to unpack tuple with indices
    template <typename Tuple, typename Func, std::size_t... Indices>
    void for_each_tuple_impl(Tuple &&tuple, Func &&func, std::index_sequence<Indices...>)
    {
        (func.template operator()<Indices>(std::get<Indices>(tuple)), ...);
    }

    // Public interface
    template <typename Tuple, typename Func>
    void for_each_tuple(Tuple &&tuple, Func &&func)
    {
        constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        for_each_tuple_impl(std::forward<Tuple>(tuple), std::forward<Func>(func), std::make_index_sequence<N>{});
    }
}