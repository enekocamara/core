#include "AttributeLayoutList.hpp"

namespace Syris::renderer{
    AttributeLayoutList::AttributeLayoutList(std::span<AttributeLayout::CreateInfo> layouts_info, uint32_t attribute_index_padding):m_attribute_index_padding(attribute_index_padding){
        for (auto layout_info : layouts_info){
            AttributeLayout layout{
                .index_layout = static_cast<uint32_t>(m_layouts.size()) + m_attribute_index_padding,
                .values_count = layout_info.values_count,
                .value_type = layout_info.value_type,
                .perInstance = layout_info.perInstance,
                .normalize = layout_info.normalize,
                .skip_size = (void *)m_stride_size,
            };

            m_stride_size += sizeof_ValueType(layout.value_type) * layout_info.values_count; 
            m_layouts.push_back(layout);
        }
        //stride for all
        for (auto& layout : m_layouts){
            layout.stride_size = m_stride_size;
        }
    }
    AttributeLayoutList::~AttributeLayoutList(){

    };
    void AttributeLayoutList::push_back(AttributeLayout::CreateInfo layout_info){
        AttributeLayout layout{
            .index_layout = static_cast<uint32_t>(m_layouts.size()) + m_attribute_index_padding,
            .values_count = layout_info.values_count,
            .value_type = layout_info.value_type,
            .perInstance = layout_info.perInstance,
            .normalize = layout_info.normalize,
            .skip_size = (void *)m_stride_size,
        };
        m_stride_size += sizeof_ValueType(layout.value_type) * layout_info.values_count;  
        for (auto& layout : m_layouts){
            layout.stride_size = m_stride_size;
        }
    }
}

