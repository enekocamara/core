#pragma  once
#include <unordered_map>

namespace Syris{
    template <typename First, typename Second, template <typename, typename> class Map = std::unordered_map>
    class BiMap
    {
    public:
        template <typename T>
        auto& from(){
            if constexpr(std::is_same<T, First>())
                return first_to_second;
            else
                return second_to_first;
        };
        template <typename T>
        const auto& from() const{
            if constexpr(std::is_same<T, First>())
                return first_to_second;
            else
                return second_to_first;
        };
        void add(First first, Second second){
            first_to_second[first] = second;
            second_to_first[second] = first;        
        }
    private:
        Map<First, Second> first_to_second;
        Map<Second, First> second_to_first;
    };
}