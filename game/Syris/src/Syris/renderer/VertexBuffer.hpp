#pragma once
#include "AttributeLayoutList.hpp"
namespace Syris::renderer{

    class VertexBuffer{
        public:
            /*
             * AttributeLayoutList layout_list;
             * void *data;
             * uint32_t size;
             */ 
            struct BufferInfo{
                AttributeLayoutList layout_list;
                uint32_t size;
                void *data;
            };

            /*
             * @brief members:
             *
             * bool dynamic;
             * std::span<BufferInfo> buffers_info;
             */ 
            struct CreateInfo{
                bool dynamic;
                std::span<BufferInfo> buffers_info;
            };
            /*
             * @brief returns a memory owning pointer to a vertex buffer
             *
             * the pointer is an abstrabtion over the current render api target
             * vertex buffer
             */
            static VertexBuffer* create(CreateInfo info);
            virtual void bind(uint32_t index) = 0;
            virtual void unbind(uint32_t index) = 0;
            virtual ~VertexBuffer(){};
    };
}
