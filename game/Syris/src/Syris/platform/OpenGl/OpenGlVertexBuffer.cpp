#include "OpenGLVertexBuffer.hpp"

#include <iostream>

#include "Syris/log/Log.h"
#include "Syris/statistics/Components.hpp"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"

namespace Syris { 
    OpenGLVertexBuffer::OpenGLVertexBuffer(CreateInfo info): m_statistics(info.statistics){
        
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&OpenGLVertexBuffer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<statistics::CVertexBuffer>(m_statistic_mod_ID);
        glGenVertexArrays(1, &m_vertex_array);
        if (m_vertex_array == NotSet){
            CORE_ERROR("Failed to generate vertex array");
            exit(1);
        }
        glBindVertexArray(m_vertex_array);
        m_buffers.reserve(info.buffers_info.size());
        for (auto& buffer_info : info.buffers_info){
            buffer_info.layout_list.print_info();
            m_buffers.emplace_back(buffer_info, m_vertex_array, m_statistics);
        }
        glBindVertexArray(0);
        Syris::Logger::client_info("Vertex array created");
        print_info(); 
    }

    void OpenGLVertexBuffer::render_statistics(entt::entity entity, entt::registry& registry){
        render_info();
        ImGui::Text("number of subbuffers: %d", m_buffers.size());
        if (ImGui::TreeNode("Subbuffers:")){
            for (auto& subbuffer : m_buffers){
                registry.get<statistics::CRender>(subbuffer.get_statistics()).render(subbuffer.get_statistics(), registry);
            }
            ImGui::TreePop();
        }
    }
    void OpenGLVertexBuffer::append(uint32_t index, uint64_t size, void* data){
        glBindVertexArray(m_vertex_array);
        m_buffers[index].append(size, data);
        glBindVertexArray(0);
    }
    void OpenGLVertexBuffer::print_info(){
        glBindVertexArray(m_vertex_array);
        GLint maxAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes); // Get the maximum number of vertex attributes

        std::cout << "VAO Info:\n";
        std::cout << "Vao ID: " << m_vertex_array << '\n';
        std::cout << "Max Vertex Attribs: " << maxAttributes << "\n";

        for (GLint i = 0; i < maxAttributes; ++i)
        {
            GLint  enabled;
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled); // Check if the attribute is enabled
            if (enabled)
            {
                GLint size, type, stride;
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);     // Get size of the attribute
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);     // Get type of the attribute
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride); // Get stride of the attribute

                GLint bufferBinding;
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding); // Get the currently bound buffer for the attribute

                // Print attribute information
                std::cout << "Attribute " << i << ": \n";
                std::cout << "  Enabled: " << (enabled ? "true" : "false") << "\n";
                std::cout << "  Size: " << size << "\n";
                std::cout << "  Type: " << type << "\n";
                std::cout << "  Stride: " << stride << "\n";
                std::cout << "  Buffer Binding: " << bufferBinding << "\n"; // Print buffer binding
            }
        }
        // Check if the VAO has an associated index buffer
        GLint indexBufferBinding;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indexBufferBinding); // Get the currently bound index buffer
        std::cout << "Index Buffer Binding: " << indexBufferBinding << "\n"; // Print index buffer binding
        if (indexBufferBinding != 0)
        {
            std::cout << "  The VAO knows about an index buffer.\n";
        }
        else
        {
            std::cout << "  The VAO does not have an associated index buffer.\n";
        }

        std::cout << "     /\n******** INFO FINISH ******/\n\n\n";
    }
    void OpenGLVertexBuffer::render_info(){
        glBindVertexArray(m_vertex_array);
        GLint maxAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes); // Get the maximum number of vertex attributes

        if (ImGui::TreeNode("Vao Info:")){
            ImGui::Text("ID: %d", m_vertex_array);
            ImGui::Text("Max Vertex Attribs: %d", maxAttributes);
            if (ImGui::TreeNode("Attributes:")){
                for (GLint i = 0; i < maxAttributes; ++i)
                {
                    GLint enabled;
                    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled); // Check if the attribute is enabled
                    if (enabled){
                        if (ImGui::TreeNode(std::format("{}º Attribute enabled",i).c_str()))
                        {
                            GLint size, type, stride;
                            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);     // Get size of the attribute
                            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);     // Get type of the attribute
                            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride); // Get stride of the attribute

                            GLint bufferBinding;
                            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding); // Get the currently bound buffer for the attribute

                            // Print attribute information
                            ImGui::Text("Size: %d", size);
                            ImGui::Text("Type: %d", type);
                            ImGui::Text("Stride: %d", stride);
                            ImGui::Text("Buffer Binding: %d", bufferBinding);
                            ImGui::TreePop();
                        }
                    } else
                        ImGui::Text("%dº Attribute disabled", i);
                }
                ImGui::TreePop();
            }
            // Check if the VAO has an associated index buffer
            GLint indexBufferBinding;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indexBufferBinding); // Get the currently bound index buffer
            if (indexBufferBinding != 0)
                ImGui::Text("Index Buffer %d binded", indexBufferBinding);
            else
                ImGui::Text("Index Buffer not binded");
            ImGui::TreePop();
        }
    }
    std::optional<uint64_t> OpenGLVertexBuffer::clear_subbuffer_range(uint32_t subbuffer_index, uint64_t offset, uint64_t size) {
        return m_buffers[subbuffer_index].clear_range(offset, size);
    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        //for (auto& buffer : m_buffers)
        //    glDeleteBuffers(1, &buffer);
    }

    void OpenGLVertexBuffer::bind(){
        glBindVertexArray(m_vertex_array);
    }

    void OpenGLVertexBuffer::bind_subbuffer(uint32_t index){
        m_buffers[index].bind();
    }
    void OpenGLVertexBuffer::unbind(uint32_t index){
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

    void OpenGLVertexBuffer::modify_subbuffer_range(uint32_t subbuffer_index, uint64_t offset, uint64_t size, void *data){
        m_buffers[subbuffer_index].modify_range(offset, size, data);
    }


    OpenGLSubBuffer::OpenGLSubBuffer(VertexBuffer::SubBufferInfo& info, GLuint vao, Statistics& statistics):m_vao(vao),m_info(info){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&OpenGLSubBuffer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod_ID = statistics.add_module(mod_info);
        statistics.get_registry().emplace<statistics::CSubBuffer>(m_statistic_mod_ID);
        
        m_capacity = 0;
        m_buffer_id = gen_buffer(info, true);
        m_capacity = m_info.size;
    }

    OpenGLSubBuffer::~OpenGLSubBuffer(){
        //todo free memory
    }

    void OpenGLSubBuffer::render_statistics(entt::entity entity, entt::registry& registry){
    }

    GLuint OpenGLSubBuffer::gen_buffer(VertexBuffer::SubBufferInfo& info, bool set_attributes){
        GLuint buffer_id;
        glBindVertexArray(m_vao);
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        if (info.dynamic)
            glBufferData(GL_ARRAY_BUFFER, info.size, info.data, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, info.size, info.data, GL_STATIC_DRAW);

        if (set_attributes)
            set_buffer_attributes(info, buffer_id);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR();
        return buffer_id;
    }
    void OpenGLSubBuffer::set_buffer_attributes(VertexBuffer::SubBufferInfo &info, GLuint array_buffer){
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
        for (uint32_t i = 0; i < info.layout_list.size(); i++)
        {
            auto &layout = info.layout_list[i];
            if (layout.value_type != ValueType::Float)
            {
                CORE_ERROR("TODO! Value type different to float");
                exit(1);
            }
            switch (layout.value_type)
            {
            case ValueType::Float:
                glVertexAttribPointer(layout.index_layout, layout.values_count, GL_FLOAT, layout.normalize, layout.stride_size, (void *)layout.skip_size);
                break;
            case ValueType::Int:
                glVertexAttribPointer(layout.index_layout, layout.values_count, GL_INT, layout.normalize, layout.stride_size, (void *)layout.skip_size);
                break;
            default:
                CORE_ERROR("TODO! Value type different to float or int");
                exit(1);
            }
            glEnableVertexAttribArray(layout.index_layout);
            if (layout.perInstance)
                glVertexAttribDivisor(layout.index_layout, 1);
        }
    }

    void OpenGLSubBuffer::bind(){
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
        CHECK_GL_ERROR();
    }

    /*size is the new total  size in bytes*/
    void OpenGLSubBuffer::reserve(uint64_t size){
        if (m_capacity == 0) {
            m_capacity = size;
        }
        while (size > m_capacity){
            m_capacity *= 2;
        }
        //fix this later me
        uint64_t hold = m_info.size;
        m_info.size = m_capacity;
        GLuint new_buffer = gen_buffer(m_info, false);
        m_info.size = hold;
        glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);  // Bind old buffer as source
        glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer); // Bind new buffer as destination
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_info.size);
        glDeleteBuffers(1, &m_buffer_id);
        CHECK_GL_ERROR();

        m_buffer_id = new_buffer;
        set_buffer_attributes(m_info, m_buffer_id);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void OpenGLSubBuffer::append(uint64_t size, void *data){
        if (m_info.size + size > m_capacity){
            reserve(size + m_info.size);
            GLint bufferSize = 0;
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id); // Bind the buffer whose size you want to query
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
            //std::cout << "Buffer size: " << bufferSize << " bytes" << std::endl;
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, m_info.size, size, data);
        m_info.size += size;
        CHECK_GL_ERROR();
    }
    std::optional<uint64_t> OpenGLSubBuffer::clear_range(uint64_t offset, uint64_t size){
        //we asume that size is the same for every item in this subbuffer. Needs to be reworked later
        //std::cout << "Clearing range offset: " << offset << " size: " << size << '\n';
        //std::cout << "Current size: " << m_info.size << " capacity: " << m_capacity << '\n';
        if (offset + size > m_info.size){
            throw std::runtime_error("incorrect offset or size");
        }
        if (m_info.size == size) {
           // std::cout << "Opt 1\n";
            m_info.size = 0;
            return std::nullopt;
        }
        else if (offset == m_info.size - size) {
           // std::cout << "Opt 2\n";
            m_info.size -= size;
           // std::cout << "After size: " << m_info.size << " capacity: " << m_capacity << '\n';
            return std::nullopt;
        }
        else {
           // std::cout << "Opt 3\n";
            //we take the last one and rewrite it in the one we "deleted", reduce the size of the buffer. no
            //memory reallocations are done in this operation
            glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);
            glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, m_info.size - size, offset, size);
            m_info.size -= size;
           // std::cout << "After size: " << m_info.size << " capacity: " << m_capacity << '\n';
            return m_info.size;
        }
        //todo
    }
    void OpenGLSubBuffer::modify_range(uint64_t offset, uint64_t size, void *data){
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        CHECK_GL_ERROR();
    }
}

