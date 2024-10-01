#pragma once
#include "AttributeLayoutList.hpp"
namespace Syris{

    class VertexBuffer{
        public:
            /*
             * AttributeLayoutList layout_list;
             * void *data;
             * uint32_t size;
             */ 

            struct SubBufferInfo{
                AttributeLayoutList layout_list;
                uint64_t size;
                void *data;
                bool dynamic;
            };

            /*
             * @brief members:
             *
             * bool dynamic;
             * std::span<BufferInfo> buffers_info;
             */ 
            struct CreateInfo{
                bool dynamic;
                std::span<SubBufferInfo> buffers_info;
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
            virtual void append(uint32_t index, uint64_t size, void* data) = 0;
            virtual ~VertexBuffer(){};
    };
}
