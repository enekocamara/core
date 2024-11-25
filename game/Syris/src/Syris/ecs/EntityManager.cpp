#include "EntityManager.hpp"

namespace Syris{

    struct CreateInfo
    {
        entt::registry &registry;
    };
    EntityManager::EntityManager(CreateInfo info) : m_batch_renderer_manager(info.batch_renderer_manager){

    }
    EntityManager::~EntityManager(){
        
    }
    void EntityManager::delete_entity(entt::entity entity){
        m_create_mutex.lock();
        m_registry.destroy(entity);
        m_create_mutex.unlock();
    }
   // entt::group &EntityManager::get_entities(){
   // }
}