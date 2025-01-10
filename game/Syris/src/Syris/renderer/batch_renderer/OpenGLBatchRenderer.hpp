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
#include "Syris/utils/Assert.h"
#include "Syris/utils/std/meta.h"

namespace Syris{

    template <typename... Types>
    std::vector<std::size_t> get_sizes()
    {
        // Use initializer list to expand the parameter pack and calculate sizes
        std::vector<std::size_t> sizes = {sizeof(Types)...};
        std::printf("Number of types: %u, number in vector: %u\n",sizeof...(Types), sizes.size());
        return {sizeof(Types)...};
    }

    template <typename... Types>
    class OpenGLBatchRenderer : public BatchRenderer
    {
    public:
        OpenGLBatchRenderer(CreateInfo& info) : m_name(info.name),
            m_shader_manager(info.shader_manager),
            m_shader_id(info.shader_id),
            m_vertex_buffer({info.layout->get_subbufers(), info.statistics}),
            m_index_buffer(info.index_buffer_info),
            m_layout(std::move(info.layout)),
            m_instance_count(0),
            m_requests_queues(get_sizes<Types...>())
        {
            Syris::Statistics::AddModuleInfo mod_info{
                .render = std::bind(&OpenGLBatchRenderer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)};
            m_statistic_mod_ID = info.statistics.add_module(mod_info);
            info.statistics.get_registry().emplace<statistics::CBatchRenderer>(m_statistic_mod_ID, m_name);
            info.statistics.add_child(m_statistic_mod_ID, m_layout->get_statistics());
        }
        ~OpenGLBatchRenderer(){
        }

        void add_entity(BR_Request &request) override{
            m_requests_queues.add_entity(request);
        }
        void set_entity(BR_RequestSparse &request) override{
            m_requests_queues.set_entity(request);
        }
        void remove_entity(BR_RemoveRequest &request) override{
            m_requests_queues.remove_entity(request);
        }

        void draw(Uniform *shader_uniforms) override{
            // m_vertex_buffers.print_info();
            ASSERT(check_integrity(), "Inconsistant inctance count");
            flush_remove_entity();
            ASSERT(check_integrity(), "Inconsistant inctance count");
            flush_add_entity();
            ASSERT(check_integrity(), "Inconsistant inctance count");
            flush_set_entity();
            flush_set_attributes();
            ASSERT(check_integrity(), "Inconsistant inctance count");
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

        bool check_integrity(){
            if (sizeof...(Types) == 0)
                return true;
            else
                return m_instance_count == m_entity_to_index.size() && m_instance_count == m_index_to_entity.size();
        }

    private:
        void flush_add_entity(){
            
            auto [add_requests, lock] = m_requests_queues.get_add_queue();
            auto& [add_queues, request_count] = add_requests;
            uint64_t start = m_instance_count;
            m_instance_count += request_count;
            if (add_queues.size() == 0 || add_queues[0].m_entity_to_index.size() == 0){
                m_requests_queues.clear_add_queue(std::move(lock));
                return;           
            }
            {
                
                std::size_t size = 0;
                for (auto &queue : add_queues)
                {
                    if (size != 0)
                        ASSERT(queue.m_entity_to_index.size() == size, "must have the same amount of request in all buffers");
                    else
                        size = queue.m_entity_to_index.size();
                }
                
            }
            //all queues should have same size and same entity id linked so we can do it once before mem copy
            for (auto [entity, index] : add_queues[0].m_entity_to_index){
                m_entity_to_index[entity] = start + index;
                m_index_to_entity[start + index] = entity;
            }
            
            std::size_t queue_index = 0;
            for (auto &request_buffer : add_queues){
                m_vertex_buffer.append(queue_index + 1, request_buffer.m_request_data.size(), request_buffer.m_request_data.data());
                queue_index++;
                //+1 to skip the first buffer, its reserved
            }
            m_requests_queues.clear_add_queue(std::move(lock));
            ASSERT(check_integrity(), "Inconsistant inctance count");
            return; 

            /*
             for (auto [entity, index] : add_requests.m_entity_to_index)
             {
                 m_entity_to_index[entity] = start + index;
                 m_index_to_entity[start + index] = entity;
             }

             for (int i = 0; i < sizeof...(Types);i++) {
                 m_vertex_buffer.append(i + 1, add_requests.m_request_data.size() * meta::size_of_type_at<Types...>(i), add_requests.m_request_data.data());
                 //+1 to skip the first buffer, its reserved
             }

             m_instance_count += add_requests.m_entity_to_index.size();
             m_requests_queues.clear_add_queue(std::move(lock));
             ASSERT(m_instance_count == m_entity_to_index.size() && m_instance_count == m_index_to_entity.size(), "Inconsistant inctance count");
             */
        }
        void flush_set_entity(){
            
            auto [set_queues_requests, lock] = m_requests_queues.get_set_queue();
            
            auto [requests_queues, request_count] = set_queues_requests;
            if (request_count == 0)
                return;
            
            std::size_t queue_index = 1;//skip first because its reserved
            for (auto& queue : requests_queues){
                for (auto [entity, index] : queue.m_entity_to_index){
                    m_vertex_buffer.modify_subbuffer_range(queue_index, m_entity_to_index[entity] * queue.m_type_size, queue.m_type_size, queue.m_request_data.data() + index * queue.m_type_size);
                }
                queue_index++;
            }
            
            
            m_requests_queues.clear_set_queue(std::move(lock));
        }
        void flush_remove_entity()
        {
            auto [remove_requests, lock] = m_requests_queues.get_remove_queue();
            if (remove_requests.size() == 0) {
                m_requests_queues.clear_remove_queue(std::move(lock));
                return;
            }
            for (entt::entity entity : remove_requests) {

                ASSERT(check_integrity(), "Inconsistant inctance count");

                if constexpr (sizeof...(Types) != 0) {

                    auto it = m_entity_to_index.find(entity);
                    ASSERT(it != m_entity_to_index.end(), "Coundn't find entity on entity to index map");
                    std::optional<std::size_t> mem_has_been_moved = std::nullopt;//stores the moved entities index
                    for (std::size_t i = 0; i < sizeof...(Types); i++) {

                        std::size_t size = m_type_sizes[i];
                        std::optional<uint64_t> moved_mem = m_vertex_buffer.clear_subbuffer_range(i + 1, it->second * size, size);
                        //+1 because the subbuffer 0 is reserved

                        if (moved_mem.has_value() && !mem_has_been_moved.has_value())
                            mem_has_been_moved = moved_mem.value() / size; //index of the entity that has been moved should be the same for all
                        else if (moved_mem.has_value() && mem_has_been_moved.has_value()) {
                            ASSERT(moved_mem.value() / size == mem_has_been_moved.value(), "inconsistant indexing throw buffers");
                        }
                    }
                    if(mem_has_been_moved.has_value()){
                            uint32_t moved_index = mem_has_been_moved.value();
                            entt::entity moved_entity = m_index_to_entity[moved_index];
                            m_index_to_entity[m_entity_to_index[entity]] = moved_entity;
                            m_entity_to_index[moved_entity] = m_entity_to_index[entity];
                            ASSERT(m_index_to_entity.find(moved_index) != m_index_to_entity.end(), "Index doesn't exist in the map");
                            ASSERT(m_entity_to_index.find(entity) != m_entity_to_index.end(), "Entity doesn't exist in the map");
                            m_index_to_entity.erase(moved_index);
                            //m_entity_to_index.erase(entity);
                    }else
                        m_index_to_entity.erase(m_entity_to_index[entity]);
                    m_entity_to_index.erase(entity);
                }
                m_instance_count--;
                ASSERT(check_integrity(), "Inconsistant inctance count");               
            }
            ASSERT(check_integrity(), "Inconsistant inctance count");
            m_requests_queues.clear_remove_queue(std::move(lock));
            
        }

        void flush_set_attributes(){
            auto [values, lock] = m_requests_queues.get_attributes();
            if (values.size() != 0)
                m_vertex_buffer.append(0, values.size() * sizeof(float), values.data());
            m_requests_queues.clear_attributes(std::move(lock));
        }
        std::string m_name;
        StatisticModID m_statistic_mod_ID;

        ShaderManager &m_shader_manager;
        ShaderManager::ShaderID m_shader_id;

        OpenGLIndexBuffer m_index_buffer;
        OpenGLVertexBuffer m_vertex_buffer;
        std::unique_ptr<BatchRendererLayout> m_layout;
        std::size_t m_instance_count;

        // every entity has a mem offset in  the vertex subbuffer 1
        std::unordered_map<entt::entity, uint32_t> m_entity_to_index;
        std::unordered_map<uint32_t, entt::entity> m_index_to_entity;
        std::vector<std::size_t> m_type_sizes = get_sizes<Types...>();
        BR_RequestQueueSystem<Types...> m_requests_queues;
    };
}