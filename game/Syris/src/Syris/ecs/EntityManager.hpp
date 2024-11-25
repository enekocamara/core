#pragma once

#include <mutex>
#include <optional>

#include <entt.hpp>

#include "Syris/renderer/batch_renderer/BatchRendererManager.hpp"

namespace Syris{
    class EntityManager{
        public:
            struct RenderInfo{
                BatchRendererManager::BR_ID renderer;
                void* entity_data;
            };
            struct EntityInfo{
                std::optional<RenderInfo> render_info;
            };
            struct CreateInfo{
                //entt::registry &registry;
                BatchRendererManager& batch_renderer_manager;
            };

            EntityManager(CreateInfo info);
            ~EntityManager();

            EntityManager() = delete;
            EntityManager(const EntityManager& ref) = delete;
            EntityManager& operator=(const EntityManager& ref) = delete;

            entt::entity new_entity(EntityInfo& info){
                m_create_mutex.lock();
                entt::entity id = m_registry.create();

                m_create_mutex.unlock();
                if (info.render_info.has_value()) {
                    RenderInfo render = info.render_info.value();
                   BR_AddRequest request{
                        .entity = id,
                        .data = render.entity_data
                    };
                    m_batch_renderer_manager.get_renderer(render.renderer)->add_entity(request);
                    //add to the material its rendering information
                    //todo
                }
                return id;
            }
            void delete_entity(entt::entity entity);
            entt::registry& get_registry(){return m_registry;}

            Syris::BatchRendererManager& get_batch_renderer_manager() {return m_batch_renderer_manager;}
           // entt::group& get_entities();
        private:
            entt::registry m_registry;
            uint32_t m_entity_count;
            std::mutex m_create_mutex;
            BatchRendererManager& m_batch_renderer_manager;

    };
}