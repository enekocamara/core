#include "EntityComponentSystem.h"
namespace ECS {
// ECS
EntityComponentSystem::~EntityComponentSystem() {
  for (auto &entities : this->entities) {
    for (auto &entity : entities) {
      delete entity;
    }
    entities.clear();
  }
}
void EntityComponentSystem::pushEntity(IEntityTexture *entity,
                                       EntityHeightFlag flag) {
  this->entities[static_cast<size_t>(flag)].push_back(entity);
}
void EntityComponentSystem::insertEntities(std::span<IEntityTexture *> entities,
                                           EntityHeightFlag flag) {
  size_t index = static_cast<size_t>(flag);
  this->entities[index].insert(this->entities[index].end(), entities.begin(),
                               entities.end());
}
const EntityArray &EntityComponentSystem::getEntities() const {
  return this->entities;
}
EntityArray &EntityComponentSystem::getEntities() { return this->entities; }
IEntityTexture *EntityComponentSystem::getEntity(glm::u32 id) {
  for (auto &entities : this->entities) {
    auto it = std::find_if(
        entities.begin(), entities.end(),
        [id](const IEntityTexture *entity) { return id == entity->getId(); });
    if (it != entities.end()) {
      return *it;
    }
  }
  return nullptr;
}
Player *EntityComponentSystem::getPlayer() {
  if (this->player_id == player_id_undefined)
    return nullptr;
  return reinterpret_cast<Player *>(this->getEntity(this->player_id));
}
void EntityComponentSystem::setPlayerId(glm::u32 id) { this->player_id = id; }

// KEYS
void EntityComponentSystem::pushKeyEntity(IEntityKey *entity) {
  this->key_entities.push_back(entity);
}
void EntityComponentSystem::insertKeyEntities(
    std::span<IEntityKey *> entities) {
  this->key_entities.insert(this->key_entities.end(), entities.begin(),
                            entities.end());
}
std::vector<IEntityKey *> &EntityComponentSystem::getKeyEntities() {
  return this->key_entities;
}
const std::vector<IEntityKey *> &EntityComponentSystem::getKeyEntities() const {
  return this->key_entities;
}
IEntityKey *EntityComponentSystem::getKeyEntity(glm::u32 id) {
  auto it = std::find_if(
      this->key_entities.begin(), this->key_entities.end(),
      [id](const IEntityKey *entity) { return id == entity->getId(); });
  if (it != this->key_entities.end()) {
    return *it;
  }
  return nullptr;
}

// RUN TICK
void EntityComponentSystem::pushRunTickEntity(IRunTickBehaviour *entity) {
  this->tick_behaviours.push_back(entity);
}
void EntityComponentSystem::insertRunTickEntities(
    std::span<IRunTickBehaviour *> entities) {
  this->tick_behaviours.insert(this->tick_behaviours.end(), entities.begin(),
                               entities.end());
}
std::vector<IRunTickBehaviour *> &EntityComponentSystem::getRunTickEntities() {
  return this->tick_behaviours;
}
const std::vector<IRunTickBehaviour *> &
EntityComponentSystem::getRunTickEntities() const {
  return this->tick_behaviours;
}
IRunTickBehaviour *EntityComponentSystem::getRunTickEntity(glm::u32 id) {
  auto it = std::find_if(
      this->tick_behaviours.begin(), this->tick_behaviours.end(),
      [id](const IRunTickBehaviour *entity) { return id == entity->getId(); });
  if (it != this->tick_behaviours.end()) {
    return *it;
  }
  return nullptr;
}
} // namespace ECS
