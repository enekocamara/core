#include "CollectableManager.hpp"

namespace Sandbox::ecs{
    CollectableManager::ID CollectableManager::set_collectable(std::vector<Stages>&& collectable){
        ID id = m_current_id++;
        m_collectables[id] = collectable;
        return id; 
    }
    const std::vector<Stages>& CollectableManager::get_collectable(ID id){
        return m_collectables[id];
    }
    void CollectableManager::remove_collectable(ID id){
        m_collectables.erase(id);
    }
}