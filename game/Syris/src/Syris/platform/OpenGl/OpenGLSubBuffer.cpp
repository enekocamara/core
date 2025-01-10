#include "OpenGLSubBuffer.hpp"

#include <iostream>

#include "Syris/platform/OpenGl/OpenGLErrors.hpp"
#include "Syris/types/OpenGLToSyrisTypes.h"
#include "Syris/statistics/Components.hpp"
#include "Syris/types/RenderTypes.h"

namespace Syris{
    OpenGLSubBuffer::OpenGLSubBuffer(VertexBuffer::SubBufferInfo info, GLuint vao, Statistics& statistics, uint32_t attribute_index_padding)
        : m_vao(vao),
          m_attribute_index_padding(attribute_index_padding),
          m_info(info){
        Syris::Statistics::AddModuleInfo mod_info{
            .render = std::bind(&OpenGLSubBuffer::render_statistics, this, std::placeholders::_1, std::placeholders::_2)
        };
        m_statistic_mod_ID = statistics.add_module(mod_info);
        statistics.get_registry().emplace<statistics::CSubBuffer>(m_statistic_mod_ID);
        
        m_capacity = 0;
        m_size = 0;
        m_buffer_id = gen_buffer(info, true, 0);
    }

    OpenGLSubBuffer::~OpenGLSubBuffer(){
        //todo free memory
    }

    void OpenGLSubBuffer::render_statistics(entt::entity entity, entt::registry& registry){
        ImGui::Text("Binding %d", m_buffer_id);
        if (ImGui::TreeNode(std::format("Attributes({})", m_info.get_attributes().size()).c_str())) {
            for (auto& attribute : m_info.get_attributes()){
                ImGui::Text(std::format("{} : {};", attribute.name, type::to_str(attribute.type)).c_str());
            }
            ImGui::TreePop();
        }
        std::format("Memory");
        if (ImGui::TreeNode("Memory"))
        {
            bind();
            ImGui::Text("Size: %u", m_size);
            ImGui::Text("Capacity: %u", m_capacity);

            if(ImGui::TreeNode("In memory values"))
            {
                std::vector<std::byte> buffer_data(m_size);

                // Read data from the GPU buffer to the CPU
                glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_size, buffer_data.data());

                // Print the buffer data (assuming float data here for demonstration)
                uint32_t max_iters = m_size / m_info.get_type_size();
                for (uint32_t iter = 0; iter < max_iters; iter++)
                {
                    uint64_t padding = 0;
                    for (auto &attrib : m_info.get_attributes())
                    {
                        type::render_type(attrib.type, buffer_data.data() + iter * m_info.get_type_size() + padding);
                        padding += dyn_sizeof_type(attrib.type);
                    }
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    GLuint OpenGLSubBuffer::gen_buffer(VertexBuffer::SubBufferInfo& info, bool set_attributes, uint64_t size){
        GLuint buffer_id;
        glBindVertexArray(m_vao);
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        if (info.is_dynamic())
            glBufferData(GL_ARRAY_BUFFER,  size, nullptr, GL_STREAM_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);

        if (set_attributes)
            set_buffer_attributes(info, buffer_id);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR();
        return buffer_id;
    }

    void OpenGLSubBuffer::set_buffer_attributes(VertexBuffer::SubBufferInfo &info, GLuint array_buffer){
        auto& openGL_map = OpenGLTypes::types().from<Type>();
        uint32_t attrib_index = m_attribute_index_padding;
        uint64_t stride = 0;
        uint64_t skip_size = 0;
        for (auto& attribute : info.get_attributes()){
            stride += dyn_sizeof_type(attribute.type);
        }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer);

        std::function<void(Type)> reqursiv_attrib = [&](Type type){
            UncopledType uncopled = uncople_type(type);
            if (!type_is_primitive(uncopled.type)){
                for (int i = 0; i < uncopled.count; i++) {
                    reqursiv_attrib(uncopled.type);
                }
            } else {
                glVertexAttribPointer(attrib_index, uncopled.count, openGL_map.at(uncopled.type), false, stride, (void *)skip_size);
                glEnableVertexAttribArray(attrib_index);
                if (info.is_per_instance())
                    glVertexAttribDivisor(attrib_index, 1);
                attrib_index++;
                skip_size += dyn_sizeof_type(uncopled.type) * uncopled.count;
            }
        };
        for (auto& attribute : info.get_attributes()){
            reqursiv_attrib(attribute.type);
        }
    }

    void OpenGLSubBuffer::bind(){
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
        CHECK_GL_ERROR();
    }

    /*size is the new total  size in bytes*/
    void OpenGLSubBuffer::reserve(uint64_t size){
        uint64_t new_capacity = m_capacity;
        if (new_capacity == 0)
            new_capacity = size;
        else {
            while (size > new_capacity) {
                new_capacity *= 2;
            }
        }
        
        GLuint new_buffer = gen_buffer(m_info, false, new_capacity);
        glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);  // Bind old buffer as source
        glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer); // Bind new buffer as destination
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_size);
        glDeleteBuffers(1, &m_buffer_id);
        CHECK_GL_ERROR();

        m_buffer_id = new_buffer;
        set_buffer_attributes(m_info, m_buffer_id);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_capacity = new_capacity;
    }
    void OpenGLSubBuffer::append(uint64_t size, void *data){
        if (m_size + size > m_capacity){
            reserve(size + m_size);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, m_size, size, data);
        m_size += size;
        CHECK_GL_ERROR();
    }
    std::optional<uint64_t> OpenGLSubBuffer::clear_range(uint64_t offset, uint64_t size){
        //we asume that size is the same for every item in this subbuffer. Needs to be reworked later
        //std::cout << "Clearing range offset: " << offset << " size: " << size << '\n';
        //std::cout << "Current size: " << m_info.size << " capacity: " << m_capacity << '\n';
        if (offset + size > m_size){
            BREAK_POINT("Incorrect offset or size");
        }
        if (m_size == size) {
           // std::cout << "Opt 1\n";
            m_size = 0;
            return std::nullopt;
        }
        else if (offset == m_size - size) {
           // std::cout << "Opt 2\n";
            m_size -= size;
           // std::cout << "After size: " << m_info.size << " capacity: " << m_capacity << '\n';
            return std::nullopt;
        }
        else {
           // std::cout << "Opt 3\n";
            //we take the last one and rewrite it in the one we "deleted", reduce the size of the buffer. no
            //memory reallocations are done in this operation
            glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);
            glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, m_size - size, offset, size);
            m_size -= size;
            return m_size;
        }
        //todo
    }
    void OpenGLSubBuffer::modify_range(uint64_t offset, uint64_t size, void *data){
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        CHECK_GL_ERROR();
    }

}