#pragma once

#include "BatchRenderer.hpp"
#include "Syris/platform/OpenGl/OpenGLIndexBuffer.hpp"
#include "Syris/statistics/Components.hpp"
#include "Syris/renderer/batch_renderer/BR_RequestQueueSystem.hpp"
/*
namespace Syris{
    class OpenGLBatchRenderer: public BatchRenderer {
    public:
        OpenGLBatchRenderer(CreateInfo &info);
    
        void add_entity(BR_DynAddRequest& request)override;
        void set_entity(BR_DynSetRequest& request)override;
        void remove_entity(BR_DynRemoveRequest& request)override;
        void set_attribute(BR_DynSetAttributeRequest& request) override;
        void draw(Uniform* shader_uniforms)override;
        uint64_t get_entity_count()override{return m_entity_count;}
        StatisticModID get_statistics()override{return m_statistic_mod_ID;}
        void render_statistics(entt::entity entity, entt::registry& registry);
    private:
        void flush_add_entity();
        void flush_set_entity();
        void flush_remove_entity();
        void flush_set_attribute();

        std::string m_name;
        StatisticModID m_statistic_mod_ID;

        ShaderManager &m_shader_manager;
        ShaderManager::ShaderID m_shader_id;

        OpenGLIndexBuffer m_index_buffer;
        OpenGLVertexBuffer m_vertex_buffer;

        uint32_t m_entity_count;
        BatchRendererLayout m_layout;
        uint64_t m_type_size;

        std::unordered_map<entt::entity, uint32_t> m_entity_to_index;
        std::unordered_map<uint32_t, entt::entity> m_index_to_entity;
       BR_RequestQueueSystem m_requests_queues;
    };
}
*/