#include "Scene.hpp"
#include "../world_generator/world_generator.hpp"
namespace scene{
    Scene::Scene(entt::registry& registry) : m_registry(registry){
        world_generator::generateGround<this->m_map_config>(this->m_registry);
    }
    Scene::~Scene(){
    }

}
