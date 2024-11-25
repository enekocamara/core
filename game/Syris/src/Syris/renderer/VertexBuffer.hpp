#pragma once
#include "AttributeLayoutList.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/types/Type.h"
#include "Syris/utils/memory.h"
namespace Syris{

    class VertexBuffer{
        public:
            /*
             * AttributeLayoutList layout_list;
             * void *data;
             * uint32_t size;
             */
            struct Attribute{
                std::string name;
                Type type;
            };

            class SubBufferInfo{
            public:
                SubBufferInfo(bool per_instance, bool dynamic) : m_per_instance(per_instance), m_dynamic(dynamic) {}
                void push(Attribute &&attribute)
                {
                    m_attributes.emplace_back(attribute);
                    m_type_size += dyn_sizeof_type(attribute.type);
                }
                bool is_dynamic()const {return m_dynamic;}
                bool is_per_instance() const {return m_per_instance;}
                uint64_t get_type_size()const{return m_type_size;}
                const std::vector<Attribute>& get_attributes()const {return m_attributes;}

            private:
                std::vector<Attribute> m_attributes;
                bool m_per_instance;
                bool m_dynamic;
                uint64_t m_type_size = 0;
            };

            /*
             * @brief members:
             *
             * bool dynamic;
             * std::span<BufferInfo> buffers_info;
             */ 
            struct CreateInfo{
                const std::vector<SubBufferInfo>& buffers_info;
                Statistics& statistics;
                //MemSpan temp; 
            };
            /*
             * @brief returns a memory owning pointer to a vertex buffer
             *
             * the pointer is an abstrabtion over the current render api target
             * vertex buffer
             */
            static VertexBuffer* create(CreateInfo info);
            virtual void bind() = 0;
            virtual void bind_subbuffer(uint32_t index) = 0;
            virtual void unbind(uint32_t index) = 0;
            virtual void append(uint32_t index, uint64_t size, void* data) = 0;
            virtual void print_info() = 0;
            virtual StatisticModID get_statistics() = 0;
            virtual ~VertexBuffer(){};
    };
}
