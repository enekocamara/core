#pragma once
#include "Syris/renderer/VertexBuffer.hpp"

namespace Syris{
    class OpenGLSubBuffer{
        public:
            OpenGLSubBuffer(VertexBuffer::SubBufferInfo info, GLuint vao, Statistics& statistics, uint32_t attribute_index_padding);
            ~OpenGLSubBuffer();
            uint64_t size();
            uint64_t instance_count();
            void bind();
            void reserve(uint64_t size);
            void append(uint64_t size, void *data);
            std::optional<uint64_t> clear_range(uint64_t memoffset, uint64_t size);
            void modify_range(uint64_t offset, uint64_t size, void *data);
            GLuint gen_buffer(VertexBuffer::SubBufferInfo& info, bool set_attributes,  uint64_t size);
            void set_buffer_attributes(VertexBuffer::SubBufferInfo& info, GLuint buffer_array);
            StatisticModID get_statistics(){return m_statistic_mod_ID;}
            void render_statistics(entt::entity entity, entt::registry& registry);
        private:
            StatisticModID m_statistic_mod_ID;
            uint32_t m_attribute_index_padding;
            GLuint m_vao;
            uint64_t m_capacity;
            uint64_t m_size;
            GLuint m_buffer_id;
            VertexBuffer::SubBufferInfo m_info;
    };
}