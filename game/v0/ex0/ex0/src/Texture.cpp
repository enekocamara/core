#include "Texture.h"
namespace ecs {
TextureAtlas::TextureAtlas() {}
TextureAtlas::~TextureAtlas() {}
void TextureAtlas::init() {
  Texture2D texture_grass = LoadTexture("textures/sprites/Tilesets/Grass.png");
  Texture2D texture_water = LoadTexture("textures/sprites/Tilesets/Water.png");
  Texture2D texture_player = LoadTexture("textures/sprites/characters/BasicCharakterSpritesheet.png");
  Texture2D texture_bush = LoadTexture("textures/sprites/objects/Basic_Grass_Biom_things.png");
  if (texture_player.id == 0 || texture_grass.id == 0 || texture_bush.id == 0 || texture_water.id == 0) {
    std::cerr << "failed to load texturs\n";
    exit(1);
  }
  this->textures[static_cast<size_t>(TextureKeys::Player)] = texture_player;
  this->textures[static_cast<size_t>(TextureKeys::Grass)] = texture_grass;
  this->textures[static_cast<size_t>(TextureKeys::Bush)] = texture_bush;
  this->textures[static_cast<size_t>(TextureKeys::Water)] = texture_water;
}

Texture TextureAtlas::getTexture(TextureKeys key) const {
  return this->textures[static_cast<size_t>(key)];
}
} // namespace ecs
