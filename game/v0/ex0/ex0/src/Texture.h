#pragma once
#include "Libs.h"
#include <array>
#include <deque>

class IEntityTexture;
namespace ECS {
    //group textures by items
    enum class TextureKeys : size_t{
        Player,
        Grass,
        Ground,
        Max
    };

    enum class TextureLayers : size_t{
        L0,
        L1,
        L2,
        Max,
    };
    
    enum class TextureTypes : size_t{
        D16x16,
        D16x32,
        D32x16,
        D32x32,
        Max
    };

    /*
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
    };*/

    struct SrcTexture{
        TextureKeys key;
        TextureTypes src_texture_type;
        glm::uvec2 src_top_left;

    };

    struct TextureBundle{
        SrcTexture src;
        glm::uvec2 size;
        glm::uvec4 color;
        float rotation;
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
            Texture getTexture(TextureKeys) const;
        private:
            std::array<std::deque<IEntityTexture*>, static_cast<size_t>(TextureLayers::Max)> layers;
            std::array<size_t, static_cast<size_t>(TextureLayers::Max)> keyToTexture;
            std::array<Texture, static_cast<size_t>(TextureKeys::Max)> textures;
            static constexpr std::array<glm::uvec2, static_cast<size_t>(TextureTypes::Max)> dimmensions = {
                glm::uvec2{16,16},
                glm::uvec2{16,32},
                glm::uvec2{32,16},
                glm::uvec2{32,32}
            };

    };

}
#include "ecs/Entity.h"

