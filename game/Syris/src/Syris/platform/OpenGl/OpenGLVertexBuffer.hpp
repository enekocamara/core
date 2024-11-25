#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include "Syris/renderer/VertexBuffer.hpp"
#include "OpenGLSubBuffer.hpp"

namespace Syris{ 
    constexpr auto NotSet = std::numeric_limits<GLuint>::max(); 
    class OpenGLVertexBuffer : public VertexBuffer{
        public:
            OpenGLVertexBuffer(CreateInfo info);
            ~OpenGLVertexBuffer();

            void bind()override;
            void bind_subbuffer(uint32_t subbuffer_index)override;
            void unbind(uint32_t subbuffer_index)override;
            void append(uint32_t subbuffer_index, uint64_t size, void* data)override;
            void modify_subbuffer_range(uint32_t subbuffer_index, uint64_t offset, uint64_t size, void *data);
            std::optional<uint64_t> clear_subbuffer_range(uint32_t subbuffer_index, uint64_t offset, uint64_t size);
            void print_info();
            void render_info();
            StatisticModID get_statistics()override{return m_statistic_mod_ID;}
            void render_statistics(entt::entity entity, entt::registry& registry);
        private:
            StatisticModID m_statistic_mod_ID;
            Statistics& m_statistics;
            GLuint m_vertex_array = std::numeric_limits<GLuint>::max();
            //std::vector<GLuint> m_buffers;
            std::vector<OpenGLSubBuffer> m_subbuffers;
    };
}
