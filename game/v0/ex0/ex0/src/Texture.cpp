#include "Texture.h"
namespace ECS {
    TextureAtlas::TextureAtlas(){
    }
    TextureAtlas::~TextureAtlas(){
    
    }
    void TextureAtlas::init(){
        Texture2D texture_grass = LoadTexture("textures/sprites/Tilesets/Grass.png");
        Texture2D texture_player = LoadTexture("textures/sprites/characters/BasicCharakterSpritesheet.png");
        if (texture_player.id == 0 || texture_grass.id == 0){
            std::cerr << "failed to load texturs\n";
            exit(1);
        }
        this->textures[static_cast<size_t>(TextureKeys::Player)] = texture_player;    
        this->textures[static_cast<size_t>(TextureKeys::Grass)] = texture_grass;

    }

    Texture TextureAtlas::getTexture(TextureKeys key) const{
        if (key != TextureKeys::Player && key != TextureKeys::Grass){
            std::cerr << "incorrect key" << static_cast<int>(key) << '\n';
            exit(1);
        }
        return this->textures[static_cast<size_t>(key)];
    }
}
