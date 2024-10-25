#pragma once
#include <vector>
#include <span>


namespace Syris{
    
    enum class ValueType{
        Float,
        Int,
        UnsignedInt,
        Byte,
        UnsignedByte,
        Short,
        UnsignedShort,
    };
    static uint32_t sizeof_ValueType(ValueType type){
        switch (type){
            case ValueType::Float: return sizeof(float);
            case ValueType::Int: return sizeof(int);
            case ValueType::UnsignedInt : return sizeof(unsigned int);
            case ValueType::Byte:
            case ValueType::UnsignedByte: return 8;
            case ValueType::Short:
            case ValueType::UnsignedShort: return 16;
        }

    };

    struct AttributeLayout{
        struct CreateInfo{
            uint32_t values_count;
            ValueType value_type;
            bool normalize;
            bool perInstance;
        };
        uint32_t index_layout;
        uint32_t values_count;
        ValueType value_type;
        bool perInstance;
        bool normalize;
        uint32_t stride_size;
        uint32_t skip_size;
    };

    class AttributeLayoutList{
        public:
            AttributeLayoutList(uint32_t attribute_index_padding):m_attribute_index_padding(attribute_index_padding){};
            AttributeLayoutList(std::span<AttributeLayout::CreateInfo> layouts, uint32_t attribute_index_padding);
            ~AttributeLayoutList();
            const AttributeLayout& operator[](uint32_t index){return m_layouts[index];}
            /*
             *@Brief pushes back a new layout, its attribute index will be the last
             */
            void push_back(AttributeLayout::CreateInfo layout_info);
            uint32_t size()const {return m_layouts.size();}
            uint32_t attribute_size(){return m_layouts.size() + m_attribute_index_padding;}
            void print_info();
        private:
            std::vector<AttributeLayout> m_layouts;
            const uint32_t m_attribute_index_padding;
            uint32_t m_stride_size = 0;
    };
}
