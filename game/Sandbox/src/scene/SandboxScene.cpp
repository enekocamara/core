#include "SandboxScene.hpp"
#include "../world_generator/world_generator.hpp"
#include "../ecs/Components.h"
#include "../ecs/Tile.h"
namespace Sandbox{
    SandboxScene::SandboxScene(entt::registry& registry):m_registry(registry){
        bool a = true;
        if (a){
            world_generator::generateGround<m_map_config>(m_registry);
        }else{
            auto texture = ecs::Tile::defaultTextureBundle();
            texture.src = texture::atlas::grass_0;
            ecs::Tile::newTile(glm::vec2(0,0), texture, registry, ecs::CTile::TileType::Water);
        }
    }
    SandboxScene::~SandboxScene(){

    }
}

