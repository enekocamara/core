#pragma once
#include <span>
#include <optional>
#include <entt.hpp>

#include "Syris/shader/ShaderManger.hpp"
#include "Syris/renderer/VertexBuffer.hpp"
#include "Syris/renderer/IndexBuffer.hpp"
#include "Syris/renderer/AttributeLayoutList.hpp"
#include "Syris/renderer/Buffer.h"
#include "Syris/statistics/Statistics.hpp"
#include "MaterialRequests.h"

namespace Syris{
    class Material{
        public:
            struct CreateInfo{
                //statististics info
                std::string name;
                //Shader::CreateInfo shader_info;
                ShaderManager& shader_manager;
                ShaderManager::ShaderID shader_id;
                VertexBuffer::CreateInfo vertex_buffer_info;
                std::optional<IndexBuffer::CreateInfo> index_buffer_info;
                std::optional<uint32_t> instance_count;
                Statistics& statistics;
            };
            virtual ~Material(){}
            virtual void add_entity(MaterialAddRequest& request)= 0;
            virtual void set_entity(MaterialSetRequest& request) = 0;
            virtual void remove_entity(MaterialRemoveRequest& request) = 0;
            virtual void draw(void *shader_uniforms) = 0;
            virtual uint64_t get_entity_count() = 0;
            virtual StatisticModID get_statistics() = 0;
                //statistics::render_childs_tree()

            /*template<class T>
            static Material* create_material(CreateInfo& info);
            */
    };
}