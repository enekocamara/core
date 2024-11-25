#pragma once
#include <cinttypes>
#include <array>
#include <string>
#include <format>
#include <expected>
#include <set>
#include <vector>
#include <list>
#include <glm/glm.hpp>
#include <variant>
#include "Syris/texture/Texture.hpp"
#include "Syris/utils/Breakpoint.h"

namespace Syris{
    enum class Type : uint64_t
    {
        f32,
        i32,
        u32,
        vec2,
        uvec2,
        ivec2,
        vec3,
        uvec3,
        ivec3,
        vec4,
        uvec4,
        ivec4,
        mat4,
        texture2D,
    };

    namespace type_map{
        static const std::array<uint64_t, 14> sizes = {
            sizeof(float),
            sizeof(int32_t),
            sizeof(uint32_t),
            sizeof(glm::vec2),
            sizeof(glm::uvec2),
            sizeof(glm::ivec2),
            sizeof(glm::vec3),
            sizeof(glm::uvec3),
            sizeof(glm::ivec3),
            sizeof(glm::vec4),
            sizeof(glm::uvec4),
            sizeof(glm::ivec4),
            sizeof(glm::mat4),
            sizeof(Syris::Texture2D)
        };

        template <Type T>
        struct TypeMap;

        template <>
        struct TypeMap<Type::f32>
        {
            using type = float;
        };
        template <>
        struct TypeMap<Type::i32>
        {
            using type = int32_t;
        };
        template <>
        struct TypeMap<Type::u32>
        {
            using type = uint32_t;
        };
        template <>
        struct TypeMap<Type::vec2>
        {
            using type = glm::vec2;
        };
        template <>
        struct TypeMap<Type::uvec2>
        {
            using type = glm::uvec2;
        };
        template <>
        struct TypeMap<Type::ivec2>
        {
            using type = glm::ivec2;
        };
        template <>
        struct TypeMap<Type::vec3>
        {
            using type = glm::vec3;
        };
        template <>
        struct TypeMap<Type::uvec3>
        {
            using type = glm::uvec3;
        };
        template <>
        struct TypeMap<Type::ivec3>
        {
            using type = glm::ivec3;
        };
        template <>
        struct TypeMap<Type::vec4>
        {
            using type = glm::vec4;
        };
        template <>
        struct TypeMap<Type::uvec4>
        {
            using type = glm::uvec4;
        };
        template <>
        struct TypeMap<Type::ivec4>
        {
            using type = glm::ivec4;
        };
        template <>
        struct TypeMap<Type::mat4>
        {
            using type = glm::mat4;
        };
        template <>
        struct TypeMap<Type::texture2D>
        {
            using type = Texture2D;
        };

    }
    template <Type T>
    using RealType = typename type_map::TypeMap<T>::type;

    template <Type type>
    constexpr inline uint64_t sizeof_type()
    {
        return sizeof(RealType<type>);
    }
    namespace type
    {
        namespace tables
        {

            static const std::set<Type> uniform_types = {
                Type::f32,
                Type::i32,
                Type::u32,
                Type::vec2,
                Type::uvec2,
                Type::ivec2,
                Type::vec3,
                Type::uvec3,
                Type::ivec3,
                Type::vec4,
                Type::uvec4,
                Type::ivec4,
                Type::mat4,
                Type::texture2D,
            };
            static const std::set<Type> attribute_types = {
                Type::f32,
                Type::i32,
                Type::u32,
                Type::vec2,
                Type::uvec2,
                Type::ivec2,
                Type::vec3,
                Type::uvec3,
                Type::ivec3,
                Type::vec4,
                Type::uvec4,
                Type::ivec4,
                Type::mat4
            };
            static const std::set<Type> openGL_suppored_types = {
                Type::f32,
                Type::i32,
                Type::u32,
            };
            static const std::vector<const char *> type_to_str{
                "f32",
                "i32",
                "u32",
                "vec2",
                "uvec2",
                "ivec2",
                "vec3",
                "uvec3",
                "ivec3",
                "vec4",
                "uvec4",
                "ivec4",
                "mat4",
                "texture",
            };
        }
        [[nodiscard]] inline bool can_be_uniform(Type type){
            return tables::uniform_types.contains(type);
        }
        [[nodiscard]] inline bool can_be_attribute(Type type){
            return tables::attribute_types.contains(type);
        }
        [[nodiscard]] inline bool is_primitive(Type type){
            return type == Type::f32 || type == Type::i32 || type == Type::u32;
        }
        [[nodiscard]] inline bool is_composed(Type type){
            return !is_primitive(type);
        }

        [[nodiscard]] inline const char *to_shader_str();
        [[nodiscard]] inline const char *to_str(Type type){
            return tables::type_to_str[(int)type];
        }
    }
    inline uint64_t dyn_sizeof_type(Type type){
        if (type == Type::texture2D) 
            BREAK_POINT("CAREFULL");
        
        return type_map::sizes[(int)type];
    }

   

    struct DynTypeVal
    {
        const Type type;
        void *mem;
    };

    template <Type... types>
    constexpr std::size_t combined_size(){
        return (sizeof(RealType<types>) + ...); // Fold expression to sum sizes
    }

    template<Type type>
    struct Value{
        RealType<type> val;
    };

    template<Type type>
    struct Var{
        const char *name;
        Value<type> val;
    };

    struct DynValue{
        const Type type;
        void *data;
    };

    struct DynVar{
        char *name;
        const Type type;
        void *data;
    };

    struct DynTypeList{
        std::vector<std::variant<DynTypeList*, Type>> types;
    };

    [[nodiscard]] inline uint64_t dyn_sizeof_type(std::variant<DynTypeList*, Type> type) {
        if (std::holds_alternative<DynTypeList*>(type)) {
            return dyn_sizeof_type(std::get<DynTypeList*>(type));
        }
        else {
            return dyn_sizeof_type(std::get<Type>(type));
        }
    };

    [[nodiscard]] inline uint64_t dyn_sizeof_type(const DynTypeList* types) {
        /*std::function<uint64_t(std::variant<DynTypeList*, Type>)> sum = [](std::variant<DynTypeList*, Type> type) {
            if (std::holds_alternative<DynTypeList*>(type)) {
                return dyn_sizeof_type(std::get<DynTypeList*>(type));
            }
            else {
                return dyn_sizeof_type(std::get<Type>(type));
            }
        };*/
        uint64_t total_size = 0;
        for (auto& type : types->types) {
            total_size += dyn_sizeof_type(type);
        }
        return total_size;
    }

    struct DynTypeValList{
        DynTypeList types;
        void *mem;
        //std::vector<std::variant<DynTypeList*, Type>> types;
    };

    [[nodiscard]] inline uint64_t dyn_sizeof_type(const DynTypeValList& list){
        return dyn_sizeof_type(&list.types);
    }

    struct End;
    struct PackedDynTypeList{
        using TypeVec = std::vector<std::variant<Type, uint32_t, End>>;
        TypeVec types;
        TypeVec::const_iterator get(TypeVec::const_iterator begin, uint32_t index);
    };
    struct DynTypeValStruct{
        PackedDynTypeList types;
        std::vector<std::byte> mem;
    };
    struct DynTypeValStructVector{
        PackedDynTypeList types;
        
    };

    [[nodiscard]] inline std::expected<Type, std::string> get_type_from_type_count(Type type, uint8_t count)
    {
        switch (type)
        {
        case Type::f32:
            switch (count)
            {
            case 1:
                return Type::f32;
            case 2:
                return Type::vec2;
            case 3:
                return Type::vec3;
            case 4:
                return Type::vec4;
            default:
                return std::unexpected(std::format("f32 with unsuported count {}", count).c_str());
            }
        case Type::i32:
            switch (count)
            {
            case 1:
                return Type::i32;
            case 2:
                return Type::ivec2;
            case 3:
                return Type::ivec3;
            case 4:
                return Type::ivec4;
            default:
                return std::unexpected(std::format("i32 with unsuported count {}", count).c_str());
            }
        case Type::u32:
            switch (count)
            {
            case 1:
                return Type::u32;
            case 2:
                return Type::uvec2;
            case 3:
                return Type::uvec3;
            case 4:
                return Type::uvec4;
            default:
                return std::unexpected(std::format("u32 with unsuported count {}", count).c_str());
            }
        default:
            if (count != 1)
                return std::unexpected("type does not support counted version not equal to 1");
            else
                return type;
        }
    }

    struct UncopledType
    {
        Type type;
        uint32_t count;
    };

    [[nodiscard]] inline bool type_is_primitive(Type type)
    {
        return type == Type::i32 || type == Type::u32 || type == Type::f32;
    }

    [[nodiscard]] inline UncopledType get_primitives(Type type)
    {
        return {(Type)((int)type % 3), ((uint32_t)type / 3 + 1)};
    }

    [[nodiscard]] inline UncopledType uncople_type(Type type)
    {
        if (type_is_primitive(type))
            return {type, 1};
        else if (type == Type::mat4)
            return {Type::vec4, 4};
        else
            return get_primitives(type);
    };

    [[nodiscard]] inline UncopledType to_glsl_type(Type type)
    {
        if (type == Type::mat4)
            return {Type::vec4, 4};
        else
            return {type, 1};
    };


    struct DynStruct
    {
        std::vector<Type> types;
    };
    struct DynStructVal
    {
        std::vector<Type> types;
        void *mem;
    };

    struct DynType
    {
        void *mem;
        uint64_t size;
    };
}