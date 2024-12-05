#include "OpenGLVertexBuffer.hpp"

#include <iostream>

#include "Syris/log/Log.h"
#include "Syris/statistics/Components.hpp"
#include "Syris/types/OpenGLToSyrisTypes.h"
#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/utils/Breakpoint.h"

namespace Syris { 
    OpenGLVertexBuffer::OpenGLVertexBuffer(CreateInfo info): m_statistics(info.statistics){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&OpenGLVertexBuffer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod_ID = info.statistics.add_module(mod_info);
        info.statistics.get_registry().emplace<statistics::CVertexBuffer>(m_statistic_mod_ID);
        glGenVertexArrays(1, &m_vertex_array);
        if (m_vertex_array == NotSet)
            BREAK_POINT("Failed to generate vertex array");
        glBindVertexArray(m_vertex_array);
        m_subbuffers.reserve(info.buffers_info.size());
        uint32_t attribute_index_padding = 0;
        for (auto& buffer_info : info.buffers_info){
            m_subbuffers.emplace_back(buffer_info, m_vertex_array, m_statistics, attribute_index_padding);
            attribute_index_padding += buffer_info.get_attributes().size();
        }
        glBindVertexArray(0);
        CLIENT_INFO("Vertex array created");
    }

    void OpenGLVertexBuffer::render_statistics(entt::entity entity, entt::registry& registry){
        render_info();
        ImGui::Text("number of subbuffers: %d", m_subbuffers.size());
        if (ImGui::TreeNode("Subbuffers:")){
            int i = 0;
            for (auto& subbuffer : m_subbuffers){
                if (ImGui::TreeNode(std::format("{}º", i).c_str())){
                    registry.get<statistics::CRender>(subbuffer.get_statistics()).render(subbuffer.get_statistics(), registry);
                    ImGui::TreePop();
                }
                i++;
            }
            ImGui::TreePop();
        }
    }
    void OpenGLVertexBuffer::append(uint32_t index, uint64_t size, void* data){
        glBindVertexArray(m_vertex_array);
        m_subbuffers[index].append(size, data);
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
            if (ImGui::TreeNode("OpenGL Attributes:")){
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
                            ImGui::Text("Type: %s", openGL_to_str(type));
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
        return m_subbuffers[subbuffer_index].clear_range(offset, size);
    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        //for (auto& buffer : m_buffers)
        //    glDeleteBuffers(1, &buffer);
    }

    void OpenGLVertexBuffer::bind(){
        glBindVertexArray(m_vertex_array);
    }

    void OpenGLVertexBuffer::bind_subbuffer(uint32_t index){
        m_subbuffers[index].bind();
    }
    void OpenGLVertexBuffer::unbind(uint32_t index){
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }

    void OpenGLVertexBuffer::modify_subbuffer_range(uint32_t subbuffer_index, uint64_t offset, uint64_t size, void *data){
        m_subbuffers[subbuffer_index].modify_range(offset, size, data);
    }


}

