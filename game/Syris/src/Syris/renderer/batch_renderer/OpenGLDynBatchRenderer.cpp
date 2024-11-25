#include "OpenGLDynBatchRenderer.hpp"
/*
namespace Syris{

    OpenGLBatchRenderer::OpenGLBatchRenderer(CreateInfo &info)
        : m_name(info.name),
          m_shader_manager(info.shader_manager),
          m_shader_id(info.shader_id),
          m_entity_count(0),
          m_index_buffer(info.index_buffer_info),
          m_vertex_buffer(VertexBuffer::CreateInfo{info.layout.get_subbufers(), info.statistics}),
          m_layout(info.layout),
          //m_type_size(info.type_size),
          m_requests_queues(0)
    {
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&OpenGLBatchRenderer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)};
        m_statistic_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.add_child(m_statistic_mod_ID, m_layout.get_statistics());
    }

    void OpenGLBatchRenderer::add_entity(BR_DynAddRequest &request){
        m_requests_queues.add_entity(request);
    }

    void OpenGLBatchRenderer::set_entity(BR_DynSetRequest &request){
        m_requests_queues.set_entity(request);
    }
    
    void OpenGLBatchRenderer::remove_entity(BR_DynRemoveRequest &request){
        m_requests_queues.remove_entity(request);
    }
    
    void OpenGLBatchRenderer::draw(Uniform *shader_uniforms){
        flush_remove_entity();
        flush_add_entity();
        flush_set_entity();
        m_shader_manager.use_shader(m_shader_id, shader_uniforms);
        m_vertex_buffer.bind();
        m_index_buffer.bind();
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_entity_count);
        CHECK_GL_ERROR();
    }
    void OpenGLBatchRenderer::flush_add_entity()
    {
        DynQueue add_request = m_requests_queues.get_add_queue();
        if (add_request.m_entity_to_index.size() == 0)
        {
            m_requests_queues.clear_add_queue();
            return;
        }
        // subbuffer will need to be specified later
        uint64_t start = m_entity_count;
        for (auto [entity, index] : add_request.m_entity_to_index)
        {
            m_entity_to_index[entity] = start + index;
            m_index_to_entity[start + index] = entity;
        }

        m_vertex_buffer.append(1, add_request.m_request_data.size() * m_type_size, add_request.m_request_data.data());
        m_entity_count += add_request.m_entity_to_index.size();
        m_requests_queues.clear_add_queue();
    }
    void OpenGLBatchRenderer::flush_set_entity()
    {
        DynQueue set_requests = m_requests_queues.get_set_queue();

        for (auto &[entity, index] : set_requests.m_entity_to_index)
        {
            m_vertex_buffer.modify_subbuffer_range(1, m_entity_to_index[entity] * m_type_size, m_type_size, &set_requests.m_request_data[index]);
        }
        m_requests_queues.clear_set_queue();
    }
    void OpenGLBatchRenderer::flush_remove_entity()
    {
        std::vector<entt::entity> &remove_requests = m_requests_queues.get_remove_queue();
        if (remove_requests.size() == 0)
        {
            m_requests_queues.clear_remove_queue();
            return;
        }
        for (auto entity : remove_requests)
        {
            std::optional<uint64_t> moved_mem = m_vertex_buffer.clear_subbuffer_range(1, m_entity_to_index[entity] * m_type_size, m_type_size);
            if (moved_mem.has_value())
            {
                uint32_t moved_index = moved_mem.value() / m_type_size;
                entt::entity moved_entity = m_index_to_entity[moved_index];
                m_index_to_entity[m_entity_to_index[entity]] = moved_entity;
                m_entity_to_index[moved_entity] = m_entity_to_index[entity];

                m_index_to_entity.erase(moved_index);
                m_entity_to_index.erase(entity);
            }
            m_entity_count--;
        }
        m_requests_queues.clear_remove_queue();
    }
    void OpenGLBatchRenderer::render_statistics(entt::entity entity, entt::registry &registry)
    {
        ImGui::Text("Entity Renderer: '%s'", m_name.c_str());
        ImGui::Text("Number of entities: %d", m_entity_count);
        if (ImGui::TreeNode("VertexBuffer"))
        {
            registry.get<statistics::CRender>(m_vertex_buffer.get_statistics()).render(m_vertex_buffer.get_statistics(), registry);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Layout")){
            registry.get<statistics::CRender>(m_layout.get_statistics()).render(m_layout.get_statistics(), registry);
            ImGui::TreePop();
        }
        
    }
    void OpenGLBatchRenderer::set_attribute(BR_DynSetAttributeRequest& request) {
        throw std::runtime_error("TODO");*/
        /*
        for (auto& var : request.data){
            bool set = false;
            auto& subbuffer = m_layout->get_subbufers()[0];
            for (auto &attribute : subbuffer.get_attributes()){
                if (attribute.name == var.name && attribute.type == var.type){
                    set = true;
                    break;
                }
            }
            if (!set){
                throw std::runtime_error(std::format("Matching var not found in any subbuffer: {} : {}", var.name, type::to_str(var.type)));
            }
        }*//*
    }
    
}*/