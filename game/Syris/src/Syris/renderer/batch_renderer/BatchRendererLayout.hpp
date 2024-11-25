#pragma once
#include <unordered_map>
#include <vector>
#include <expected>
#include "Syris/shader/Layout.hpp"
#include "Syris/statistics/Statistics.hpp"
#include "Syris/renderer/VertexBuffer.hpp"
#include "Syris/shader/Shader.hpp"
namespace Syris{
    class BatchRendererLayout{

        public:
            struct CreateInfo{
                const Layout& attributes_layout;
                const Layout& uniforms_layout;
                Statistics& statistics;
            };
            BatchRendererLayout(CreateInfo info);
            BatchRendererLayout(Shader* shader, Statistics& statistics);
            BatchRendererLayout(const BatchRendererLayout& ref) = delete;
            BatchRendererLayout(BatchRendererLayout&&) = default;
            BatchRendererLayout& operator=(BatchRendererLayout&&) = default;

            BatchRendererLayout& operator=(const BatchRendererLayout& ref) = delete;
            

            std::expected<void, std::string> set_subbuffer(VertexBuffer::SubBufferInfo subbuffer);
            std::expected<void, std::string> finish();
            const std::vector<VertexBuffer::SubBufferInfo>& get_subbufers()const{return m_subbuffers;}
            void render_statistics(entt::entity entity, entt::registry& registry);
            StatisticModID get_statistics()const{return m_statistic_mod_ID;}
            const Layout& get_attribute_layout()const { return m_attribute_layout; }
            const Layout& get_uniforms_layout()const { return m_uniforms_layout; }
        private:
            std::vector<VertexBuffer::SubBufferInfo> m_subbuffers;
            std::set<std::string> m_attributes_set;
            const Layout& m_attribute_layout;
            const Layout& m_uniforms_layout;
            Statistics& m_statistics;
            StatisticModID m_statistic_mod_ID;
    };
}