#pragma once
#include "../Libs.h"
#include <array>
#include <deque>

class IEntityTexture;
namespace ecs {

    namespace textures {
        static constexpr glm::fvec2 default_tyle_size = {16.f, 16.f};
        struct Player{
            enum class Dir : int{
                Down,
                Up,
                Left,
                Right
            };
/***            static constexpr Rectangle getPlayerTextureRectangle(Dir dir, bool idle, int index){
                int layer = static_cast<int>(dir);
                int column = !(int)idle * 2 + index;
                return Rectangle{
                    .x = default_tyle_size.x * column * 3 + default_tyle_size.x,
                        .y = default_tyle_size.y * layer * 3 + default_tyle_size.y,
                        .width = default_tyle_size.x,
                        .height = default_tyle_size.y
                };
            }
            ***/
        };
        /***
        static constexpr Rectangle grass_0 = {
            .x = default_tyle_size.x,
            .y = default_tyle_size.y,
            .width = default_tyle_size.x,
            .height = default_tyle_size.y
        };
        static constexpr Rectangle bush_with_berries = {
            .x = 0,
            .y = default_tyle_size.y * 3,
            .width = default_tyle_size.x,
            .height = default_tyle_size.y
        };
        static constexpr Rectangle bush_no_berries = {
            .x = default_tyle_size.x,
            .y = default_tyle_size.y * 3,
            .width = default_tyle_size.x,
            .height = default_tyle_size.y
        };
        static constexpr Rectangle water_0 = {
            .x = 0,
            .y = 0,
            .width = default_tyle_size.x,
            .height = default_tyle_size.y
        };
***/
    };
    enum class TextureKeys : size_t{
        Player,
        Grass,
        Ground,
        Bush,
        Water,
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
    ///    Rectangle rect;
    };

    struct TextureBundle{
        TextureSrc src;
        glm::fvec2 size;
   ///     Color color;
        float rotation;
    };

    
 
    class TextureAtlas
    {
        public:
            TextureAtlas();
            ~TextureAtlas();
            ///Texture getTexture(TextureKeys) const;
            void init();
        private:
            std::array<std::deque<IEntityTexture*>, static_cast<size_t>(TextureLayers::Max)> layers;
            ///std::array<Texture, static_cast<size_t>(TextureKeys::Max)> textures;

    };
}
