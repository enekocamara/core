#pragma once
#include <iostream>

#include <memory>
namespace Syris{

    class VertexBuffer{
        public:
            struct AttributeLayout{
                uint32_t index_layout;
                uint32_t values_count;
                bool normalize;
                uint32_t stride_count;
                uint32_t skip_count;
            };
            struct CreateInfo{
                bool dynamic;
                uint32_t vertice_size;
                uint32_t vertices_count;
                void *data;
                AttributeLayout* layouts;
                uint32_t layouts_count;
            };
            static VertexBuffer* create(CreateInfo info);
            virtual void bind() = 0;
            virtual void unbind() = 0;
            virtual ~VertexBuffer(){};
    };
}
