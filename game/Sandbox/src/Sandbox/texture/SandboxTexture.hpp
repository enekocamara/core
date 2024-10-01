#pragma once
#include "Syris/renderer/Texture.h"

namespace Sandbox::texture{
    static constexpr glm::fvec2 default_tyle_size = {16.f, 16.f};
    namespace Player {
        enum class Dir : int { Down, Up, Left, Right };
        static constexpr Syris::texture::Rectangle2D getPlayerTextureRectangle(Dir dir, bool idle,
                int index) {
            int layer = static_cast<int>(dir);
            int column = !(int)idle * 2 + index;
            return Syris::texture::Rectangle2D{
                /*.orig = {default_tyle_size.x * column * 3 + default_tyle_size.x,
                  default_tyle_size.y * layer * 3 + default_tyle_size.y},
                  .size = {default_tyle_size.x, default_tyle_size.y}};*/
                .min = {0,0},
                .max = {0,0}
        };

        }
    }

    namespace atlas{
        static constexpr size_t tile_size = 16;
        static constexpr size_t actions = 0;
        static constexpr size_t player = 36;
        static constexpr size_t objects = player + 12;
        static constexpr size_t plants = objects + 6;
        static constexpr size_t crop = plants + 5;
        static constexpr size_t eggs = crop + 2;
        static constexpr size_t fences = eggs + 1;
        static constexpr size_t chickens = fences + 4;
        static constexpr size_t cows = chickens + 2;
        static constexpr size_t grass = cows + 4;
        static constexpr size_t hills = grass + 7;
        static constexpr size_t wood_paths = hills + 7;
        static constexpr size_t dirt = wood_paths + 4;
        static constexpr size_t bridges = dirt + 7;
        static constexpr size_t home = bridges + 3;
        static constexpr size_t total_y = home + 5;
        static constexpr size_t total_x = 12;

        static constexpr float nor_y(size_t index){
            return (total_y - index) / float(total_y);
        }
        static constexpr float nor_x(size_t index){
            return (index) / float(total_x);
        }

        static constexpr Syris::texture::Rectangle2D grass_0 = {
            .min = {0.f, nor_y(grass + 5)},
            .max = {nor_x(1), nor_y(grass + 6)}
        };
        static constexpr Syris::texture::Rectangle2D bush_with_berries = {
            .min = {0.f, nor_y(plants + 3)},
            .max = {nor_x(1), nor_y(plants + 4)}
        };
        static constexpr Syris::texture::Rectangle2D bush_no_berries = {
            .min = {nor_x(1), nor_y(plants + 3)},
            .max = {nor_y(2), nor_y(plants + 4)}
        };
        static constexpr Syris::texture::Rectangle2D dirt_0 = {
            .min = {0.f, nor_y(dirt + 5)},
            .max = {nor_x(1), nor_y(dirt + 6)}
        };
        static constexpr Syris::texture::Rectangle2D player_0 = {
            .min = {nor_x(1.f), nor_y(player + 1)},
            .max = {nor_x(2), nor_y(player + 2)}
        };
    }
}; // namespace textures
