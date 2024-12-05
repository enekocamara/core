#pragma once
#include <unordered_map>
#include <vector>

#include "Syris/statistics/Statistics.hpp"
#include "BatchRenderer.hpp"
#include "NewBatchRenderer.h"

namespace Syris{
    class BatchRendererManager{
        public:
            struct CreateInfo{
                Statistics& statistics;
            };
            using BR_ID = uint32_t;

            BatchRendererManager(CreateInfo info);
            ~BatchRendererManager();
            BatchRendererManager(const BatchRendererManager& ref) = delete;

            template<typename... Types>
            BR_ID add_renderer(BatchRenderer::CreateInfo& info){
                std::unique_ptr<BatchRenderer> batch_renderer = new_batch_renderer<Types...>(info);
                m_statistics.add_child(m_statistic_mod, batch_renderer->get_statistics());
                BR_ID id = m_current_id;
                m_current_id++;
                m_batch_renderers[id] = std::move(batch_renderer);
                return id;
            }

            void set_entity(BR_ID id, BR_RequestSparse& request);
            void draw(BR_ID id, Uniform  *uniforms);
            BatchRenderer* get_renderer(BR_ID id);
            StatisticModID get_statistics(){return m_statistic_mod;}
            void render(entt::entity entity, entt::registry& registry);
        private:
            std::unordered_map<BR_ID, std::unique_ptr<BatchRenderer>> m_batch_renderers;
            BR_ID m_current_id = 1;
            Statistics& m_statistics;
            StatisticModID m_statistic_mod;
    };
}
