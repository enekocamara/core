#pragma once
#include <memory>

namespace Syris{

    class IndexBuffer{
        public:
            struct CreateInfo{
                uint64_t indices_count;
                uint32_t* indices;
                bool dynamic;
            };
            virtual ~IndexBuffer(){};
            virtual void bind() = 0;
            virtual void unbind() = 0;

            /*
             * @brief returns a memory owning pointer to a index buffer
             *
             * the pointer is an abstrabtion over the current render api target
             * index buffer
             */
            static IndexBuffer* create(CreateInfo info);
    };
}
