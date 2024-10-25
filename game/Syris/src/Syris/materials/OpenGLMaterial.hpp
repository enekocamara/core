#pragma once
#include <optional>
#include <unordered_map>
#include <entt.hpp>

#include "Material.hpp"
#include "MatRequestQueueSystem.hpp"
#include "Syris/shader/OpenGLShader.hpp"
#include "Syris/platform/OpenGl/OpenGLVertexBuffer.hpp"
#include "Syris/platform/OpenGl/OpenGLIndexBuffer.hpp"
#include "Syris/utils/memory.h"
#include "Syris/include/imgui.h"
#include "Syris/statistics/Components.hpp"
//i want to combine updating data with shaders
//render buffer is a implementation of a system i want to make

namespace Syris{
    /*T is the type that subbufer 1 will hold */
    template <typename T>
    class OpenGLMaterial : public Material
    {
    public:
        OpenGLMaterial(CreateInfo &info) : m_name(info.name),
                                           m_shader_manager(info.shader_manager),
                                           m_shader_id(info.shader_id),
                                           m_vertex_buffers(info.vertex_buffer_info),
                                           m_instance_count(info.instance_count)
        {

            if (info.index_buffer_info.has_value())
            {
                m_vertex_buffers.bind();
                m_index_Buffer.emplace(info.index_buffer_info.value());

                // statistics
                Syris::Statistics::AddModuleInfo mod_info{
                    .render = std::bind(&OpenGLMaterial::render_statistics,this, std::placeholders::_1, std::placeholders::_2)
                };
                m_statistic_mod_ID = info.statistics.add_module(mod_info);
                info.statistics.get_registry().emplace<Syris::statistics::CMaterial>(m_statistic_mod_ID, info.name);
            }
            if (!m_index_Buffer.has_value() && !m_instance_count.has_value())
            {
                CORE_ERROR("Render method unsuported");
                exit(1);
            }
        }
        ~OpenGLMaterial(){
        }

        void add_entity(MaterialAddRequest &request) override{
            m_requests_queues.add_entity(request);
        }
        void set_entity(MaterialSetRequest &request) override{
            m_requests_queues.set_entity(request);
        }
        void remove_entity(MaterialRemoveRequest &request) override{
            m_requests_queues.remove_entity(request);
        }

        void draw(void *shader_uniforms) override{
            // m_vertex_buffers.print_info();
            flush_remove_entity();
            flush_add_entity();
            flush_set_entity();
            CHECK_GL_ERROR();
            if (m_index_Buffer.has_value() && m_instance_count.has_value())
            {
            //    std::cout << "rendering\n";
                m_shader_manager.use_shader(m_shader_id, shader_uniforms);
                m_vertex_buffers.bind();
                m_index_Buffer.value().bind();
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_instance_count.value());
                CHECK_GL_ERROR();
            }
            else if (m_index_Buffer.has_value() && !m_instance_count.has_value())
            {
                m_shader_manager.use_shader(m_shader_id, shader_uniforms);
                m_index_Buffer.value().bind();
                m_vertex_buffers.bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                CHECK_GL_ERROR();
            }
            else
            {
                CORE_ERROR("Render method unsuported");
                exit(1);
            }
        }
        uint64_t get_entity_count()override{return m_instance_count.value();}
        StatisticModID get_statistics(){return m_statistic_mod_ID;}
        void render_statistics(entt::entity entity, entt::registry& registry){
            ImGui::Text("Number of entities: %d", m_instance_count.value());
            if (ImGui::TreeNode("VertexBuffer")){
                registry.get<statistics::CRender>(m_vertex_buffers.get_statistics()).render(m_vertex_buffers.get_statistics(), registry);
                ImGui::TreePop();
            }
        }
    private:
        void flush_add_entity(){
            Queue<T> add_request = m_requests_queues.get_add_queue();
            if (add_request.m_entity_to_index.size() == 0) {
                m_requests_queues.clear_add_queue();
                return;
            }
            // subbuffer will need to be specified later
            uint64_t start = m_instance_count.value();
            for (auto [entity, index] : add_request.m_entity_to_index)
            {
                m_entity_to_index[entity] = start + index;
                m_index_to_entity[start + index] = entity;
            }
            //std::cout << "BEGIN FLUSH ADD:\n";
            /*for (auto& data : add_request.m_request_data) {
                data.print();
            }*/
            m_vertex_buffers.append(1, add_request.m_request_data.size() * sizeof(T), add_request.m_request_data.data());
            // m_vertex_buffers.append(subbufer_index, size, data);
           // std::cout << "adding new " << add_request.m_entity_to_index.size() << " entities\n";
            if (m_instance_count.has_value())
            {
            //    std::cout << "entities before: " << m_instance_count.value();
                m_instance_count.value() += add_request.m_entity_to_index.size();
            //    std::cout << " , after: " << m_instance_count.value() << '\n';
            }
            m_requests_queues.clear_add_queue();
        }
        void flush_set_entity(){
            Queue<T> set_requests = m_requests_queues.get_set_queue();
            
            for (auto& [entity, index] : set_requests.m_entity_to_index){
                m_vertex_buffers.modify_subbuffer_range(1, m_entity_to_index[entity] * sizeof(T), sizeof(T), &set_requests.m_request_data[index]);
                //m_entity_to_index[entity];
            }/*
            for (auto set_request : set_requests)
            {
                if (set_request.index_buffer || !set_request.sub_buffer_index.has_value())
                {
                    // TODO
                    CORE_ERROR("index buffer data change not supported yet");
                    exit(1);
                }
                else if (set_request.sub_buffer_index.has_value())
                {
                    m_vertex_buffers.bind_subbuffer(set_request.sub_buffer_index.value());
                    glBufferSubData(GL_ARRAY_BUFFER, set_request.mem_offset, set_request.size, set_request.data);
                    CHECK_GL_ERROR();
                }
            }*/
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
                std::optional<uint64_t> moved_mem = m_vertex_buffers.clear_subbuffer_range(1, m_entity_to_index[entity] * sizeof(T), sizeof(T));
                if (moved_mem.has_value()){
                    uint32_t moved_index = moved_mem.value() / sizeof(T);
                    entt::entity moved_entity = m_index_to_entity[moved_index];
                    m_index_to_entity[m_entity_to_index[entity]] = moved_entity;
                    m_entity_to_index[moved_entity] = m_entity_to_index[entity];

                    m_index_to_entity.erase(moved_index);
                    m_entity_to_index.erase(entity);
                }
                m_instance_count.value()--;
            }
            m_requests_queues.clear_remove_queue();
        }

        std::string m_name;
        StatisticModID m_statistic_mod_ID;

        ShaderManager &m_shader_manager;
        ShaderManager::ShaderID m_shader_id;
        std::optional<OpenGLIndexBuffer> m_index_Buffer;
        std::optional<uint32_t> m_instance_count;
        OpenGLVertexBuffer m_vertex_buffers;

        // every entity has a mem offset in  the vertex subbuffer 1
        std::unordered_map<entt::entity, uint32_t> m_entity_to_index;
        std::unordered_map<uint32_t, entt::entity> m_index_to_entity;
        MatRequestQueueSystem<T> m_requests_queues;
    };
}