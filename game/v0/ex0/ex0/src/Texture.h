#pragma once
#include "Libs.h"
#include <array>
#include <deque>

class IEntityTexture;
namespace ECS {
    //group textures by items

    namespace textures {
        static constexpr glm::fvec2 src_player_size = {48.f, 48.f};
        static constexpr Rectangle player_forward_default = {
            .x = 0,
            .y = 0,
            .width = src_player_size.x,
            .height = src_player_size.y
        };
        static constexpr Rectangle player_forward_idle0 = {
            .x = src_player_size.x,
            .y = 0,
            .width = src_player_size.x,
            .height = src_player_size.y
        };
    }

    enum class TextureKeys : size_t{
        Player,
        Grass,
        Ground,
        Bush,
        Max
    };

    enum class TextureLayers : size_t{
        L0,
        L1,
        L2,
        Max,
    };
    
    struct TextureSrc{
        TextureKeys key;
        Rectangle rect;
    };

    struct TextureBundle{
        TextureSrc src;
        glm::fvec2 size;
        Color color;
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
            void init();
        private:
            std::array<std::deque<IEntityTexture*>, static_cast<size_t>(TextureLayers::Max)> layers;
            std::array<Texture, static_cast<size_t>(TextureKeys::Max)> textures;

    };

}
#include "ecs/Entity.h"

