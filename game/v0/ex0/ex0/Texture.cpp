#include "Texture.h"
namespace ECS {
    TextureAtlas::TextureAtlas(){
    
    }
    TextureAtlas::~TextureAtlas(){
    
    }
    TextureAtlas& TextureAtlas::pushTexture(PrimaryKeys key, TextureInfo info){
        size_t texture_index = -1;
        for (size_t i = 0; i < this->textures.size(); i++){
            if (this->textures[i].id == info.texture.id){
                texture_index = i;
                break;
            }
        }
        if (texture_index == -1){
            texture_index = this->textures.size();
            this->textures.push_back(info.texture);
        }
        this->keyToTexture[static_cast<size_t>(key)] = texture_index;
        this->dimmensions[static_cast<size_t>(key)] = info.dimmensions;
        return *this;
    }

    TextureInfo TextureAtlas::getTexture(PrimaryKeys key) const{
        return {this->dimmensions[static_cast<size_t>(key)], this->textures[this->keyToTexture[static_cast<size_t>(key)]] };
    }
}
