#pragma once
#include <optional>
#include <unordered_map>
#include <entt.hpp>

#include "BatchRenderer.hpp"
#include "BR_RequestQueueSystem.hpp"
#include "Syris/shader/OpenGLShader.hpp"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLIndexBuffer.hpp"
#include "Syris/utils/memory.h"
#include "Syris/include/imgui.h"
#include "Syris/statistics/Components.hpp"


namespace Syris{
    /*T is the type that subbufer 1 will hold */
    template <typename T>
    class OpenGLBatchRenderer : public BatchRenderer
    {
    public:
        OpenGLBatchRenderer(CreateInfo& info) : m_name(info.name),
            m_shader_manager(info.shader_manager),
            m_shader_id(info.shader_id),
            m_vertex_buffer({info.layout->get_subbufers(), info.statistics}),
            m_index_buffer(info.index_buffer_info),
            m_layout(std::move(info.layout)),
            m_instance_count(0)
        {
            Syris::Statistics::AddModuleInfo mod_info{
                .render = std::bind(&OpenGLBatchRenderer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)};
            m_statistic_mod_ID = info.statistics.add_module(mod_info);
            info.statistics.get_registry().emplace<statistics::CBatchRenderer>(m_statistic_mod_ID, m_name);
            info.statistics.add_child(m_statistic_mod_ID, m_layout->get_statistics());
        }
        ~OpenGLBatchRenderer(){
        }

        void add_entity(BR_AddRequest &request) override{
            m_requests_queues.add_entity(request);
        }
        void set_entity(BR_SetRequest &request) override{
            m_requests_queues.set_entity(request);
        }
        void remove_entity(BR_RemoveRequest &request) override{
            m_requests_queues.remove_entity(request);
        }

        void draw(Uniform *shader_uniforms) override{
            // m_vertex_buffers.print_info();
            flush_remove_entity();
            flush_add_entity();
            flush_set_entity();
            flush_set_attributes();
            m_shader_manager.use_shader(m_shader_id, shader_uniforms);
            CHECK_GL_ERROR();
            m_vertex_buffer.bind();
            m_index_buffer.bind();
            CHECK_GL_ERROR();
            //return;
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_instance_count);
        }
        uint64_t get_entity_count()override{return m_instance_count;}

        StatisticModID get_statistics(){return m_statistic_mod_ID;}

        void render_statistics(entt::entity entity, entt::registry& registry){
            ImGui::Text("Entity Renderer: '%s'", m_name.c_str());
            ImGui::Text("Number of entities: %d", m_instance_count);
            if (ImGui::TreeNode("VertexBuffer"))
            {
                registry.get<statistics::CRender>(m_vertex_buffer.get_statistics()).render(m_vertex_buffer.get_statistics(), registry);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Layout"))
            {
                registry.get<statistics::CRender>(m_layout->get_statistics()).render(m_layout->get_statistics(), registry);
                ImGui::TreePop();
            }
        }

        void set_attribute(BR_SetAttributeRequest& request){
            m_requests_queues.set_attributes(request.values);
        }

    private:
        void flush_add_entity(){
            BR_Queue<T> add_request = m_requests_queues.get_add_queue();
            if (add_request.m_entity_to_index.size() == 0) {
                m_requests_queues.clear_add_queue();
                return;
            }
            
            uint64_t start = m_instance_count;
            for (auto [entity, index] : add_request.m_entity_to_index)
            {
                m_entity_to_index[entity] = start + index;
                m_index_to_entity[start + index] = entity;
            }
            
            m_vertex_buffer.append(1, add_request.m_request_data.size() * sizeof(T), add_request.m_request_data.data());
            
            m_instance_count += add_request.m_entity_to_index.size();
            m_requests_queues.clear_add_queue();
        }
        void flush_set_entity(){
            BR_Queue<T> set_requests = m_requests_queues.get_set_queue();
            
            for (auto& [entity, index] : set_requests.m_entity_to_index){
                m_vertex_buffer.modify_subbuffer_range(1, m_entity_to_index[entity] * sizeof(T), sizeof(T), &set_requests.m_request_data[index]);
            }
            m_requests_queues.clear_set_queue();
        }
        void flush_remove_entity()
        {
            std::vector<entt::entity>& remove_requests = m_requests_queues.get_remove_queue();
            if (remove_requests.size() == 0) {
                m_requests_queues.clear_remove_queue();
                return;
            }
            for (auto entity : remove_requests) {
                std::optional<uint64_t> moved_mem = m_vertex_buffer.clear_subbuffer_range(1, m_entity_to_index[entity] * sizeof(T), sizeof(T));
                if (moved_mem.has_value()){
                    uint32_t moved_index = moved_mem.value() / sizeof(T);
                    entt::entity moved_entity = m_index_to_entity[moved_index];
                    m_index_to_entity[m_entity_to_index[entity]] = moved_entity;
                    m_entity_to_index[moved_entity] = m_entity_to_index[entity];

                    m_index_to_entity.erase(moved_index);
                    m_entity_to_index.erase(entity);
                }
                m_instance_count--;
            }
            m_requests_queues.clear_remove_queue();
        }

        void flush_set_attributes(){
            auto& values = m_requests_queues.get_attributes();
            if (values.size() != 0)
                m_vertex_buffer.append(0, values.size() * sizeof(float), values.data());
            m_requests_queues.clear_attributes();
        }
        std::string m_name;
        StatisticModID m_statistic_mod_ID;

        ShaderManager &m_shader_manager;
        ShaderManager::ShaderID m_shader_id;

        OpenGLIndexBuffer m_index_buffer;
        OpenGLVertexBuffer m_vertex_buffer;
        std::unique_ptr<BatchRendererLayout> m_layout;
        uint32_t m_instance_count;

        // every entity has a mem offset in  the vertex subbuffer 1
        std::unordered_map<entt::entity, uint32_t> m_entity_to_index;
        std::unordered_map<uint32_t, entt::entity> m_index_to_entity;
        BR_RequestQueueSystem<T> m_requests_queues;
    };
}