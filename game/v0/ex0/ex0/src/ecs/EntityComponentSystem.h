#pragma once

#include "Entity.h"
#include "Player.h"
namespace ECS {
using EntityArray = std::array<std::vector<IEntityTexture *>,
                               static_cast<size_t>(EntityHeightFlag::Max)>;
class EntityComponentSystem {
public:
  EntityComponentSystem(){};
  ~EntityComponentSystem();

  const EntityArray &getEntities() const;
  EntityArray &getEntities();
  void pushEntity(IEntityTexture *entity, EntityHeightFlag flag);
  void insertEntities(std::span<IEntityTexture *> entities,
                      EntityHeightFlag flag);
  IEntityTexture *getEntity(glm::u32 id);
  Player *getPlayer();
  void setPlayerId(glm::u32);
  glm::u32 getPlayerId() { return player_id; }

  const std::vector<IEntityKey *> &getKeyEntities() const;
  std::vector<IEntityKey *> &getKeyEntities();
  void pushKeyEntity(IEntityKey *entity);
  void insertKeyEntities(std::span<IEntityKey *> entities);
  IEntityKey *getKeyEntity(glm::u32 id);

  const std::vector<IRunTickBehaviour *> &getRunTickEntities() const;
  std::vector<IRunTickBehaviour *> &getRunTickEntities();
  void pushRunTickEntity(IRunTickBehaviour *entity);
  void insertRunTickEntities(std::span<IRunTickBehaviour *> entities);
  IRunTickBehaviour *getRunTickEntity(glm::u32 id);

private:
  EntityArray entities;
  std::vector<IEntityKey *> key_entities;
  std::vector<IRunTickBehaviour *> tick_behaviours;
  glm::u32 player_id = std::numeric_limits<glm::u32>::max(); // not set
};
} // namespace ECS
