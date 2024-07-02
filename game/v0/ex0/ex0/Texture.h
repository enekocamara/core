#pragma once
#include "Libs.h"
#include <array>
#include <type_traits>
#include <concepts>
#include <variant>

namespace ECS {
    //group textures by items
    
    enum class PrimaryKeys : size_t{
        Player,
        Bush,
        Grass,
        Max,
    };

    struct TextureRect{
        glm::vec2 src_top_left;
        glm::vec2 src_size;
        
        Rectangle getSrcRectangle(){
            return Rectangle{src_top_left.x, src_top_left.y, src_size.x, src_size.y};
        }
    };
    struct TextureDimmensions{
        Rectangle src_rect;
        float dst_width;
        float dst_height;
    };

    struct TextureInfo{
        TextureDimmensions dimmensions;
        Texture texture;
    };
    /*
    template<typename T>
    concept TextureKeyConcept = std::is_enum_v<T>() &&
                         std::is_same_v<std::underlying_type_t<T>, std::size_t> &&
                         requires {T::Max;};


    template<TextureKeyConcept T>
    class TextureTreeNode{
        public:
            TextureTreeNode();
            ~TextureTreeNode();
        private:
        //    template <TextureKeyConcept I>
        //        std::variant<CustomTexture,std::array<TextureTreeNode<I>*,T::Max>> content;
    };
    
    */
    class TextureAtlas
    {
        public:
            TextureAtlas();
            ~TextureAtlas();
            TextureAtlas& pushTexture(PrimaryKeys key, TextureInfo info);
            TextureInfo getTexture(PrimaryKeys) const;
        private:
            std::array<size_t, static_cast<size_t>(PrimaryKeys::Max)> keyToTexture;
            std::vector<Texture> textures;
            std::array<TextureDimmensions, static_cast<size_t>(PrimaryKeys::Max)> dimmensions;
    };

}

