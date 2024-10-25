#pragma once

#include <mutex>
#include <optional>

#include <entt.hpp>

#include "Syris/materials/Material.hpp"
#include "Syris/materials/MaterialManager.hpp"

namespace Syris{
    class EntityManager{
        public:
            struct RenderInfo{
                Syris::MaterialManager::MaterialID material;
                void* entity_data;
            };
            struct EntityInfo{
                std::optional<RenderInfo> render_info;
            };
            struct CreateInfo{
                //entt::registry &registry;
                Syris::MaterialManager& material_manager;
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
                    MaterialAddRequest request{
                        .entity = id,
                        .data = render.entity_data
                    };
                    m_material_manager.get_material(render.material)->add_entity(request);
                    //add to the material its rendering information
                    //todo
                }
                return id;
            }
            void delete_entity(entt::entity entity);
            entt::registry& get_registry(){return m_registry;}

            Syris::MaterialManager& get_materials() {return m_material_manager;}
           // entt::group& get_entities();
        private:
            entt::registry m_registry;
            uint32_t m_entity_count;
            std::mutex m_create_mutex;
            Syris::MaterialManager& m_material_manager;

    };
}