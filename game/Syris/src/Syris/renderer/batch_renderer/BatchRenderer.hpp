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
#include "BR_Requests.h"
#include "BatchRendererLayout.hpp"
#include "Syris/utils/memory.h"
namespace Syris{
    class BatchRenderer{
        public:
            struct CreateInfo{
                std::string name;
                ShaderManager& shader_manager;
                ShaderManager::ShaderID shader_id;
                IndexBuffer::CreateInfo index_buffer_info;
                std::unique_ptr<BatchRendererLayout> layout;
                //uint64_t type_size;
                Statistics& statistics;
                //MemSpan temp;//temporally to fix issues
            };
            virtual ~BatchRenderer(){}
            //virtual void add_entity(BR_RequestSparse& request)= 0;
            virtual void add_entity(BR_Request& request)= 0;
            virtual void set_entity(BR_RequestSparse& request) = 0;
            virtual void remove_entity(BR_RemoveRequest& request) = 0;
            virtual void draw(Uniform *shader_uniforms) = 0;
            virtual uint64_t get_entity_count() = 0;
            virtual StatisticModID get_statistics() = 0;
            virtual void set_attribute(BR_SetAttributeRequest& request) = 0;
    };
}