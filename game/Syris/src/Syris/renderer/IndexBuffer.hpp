#pragma once
#include <memory>

namespace Syris{

    class IndexBuffer{
        public:
            struct CreateInfo{
                uint32_t indices_count;
                uint32_t* indices;
                bool dynamic;
            };
            virtual ~IndexBuffer(){};
            virtual void bind() = 0;
            virtual void unbind() = 0;
            static IndexBuffer* create(CreateInfo info);
    };
}
