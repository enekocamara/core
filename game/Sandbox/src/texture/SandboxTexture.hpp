#pragma
#include "renderer/Texture.h"

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

    static constexpr Syris::texture::Rectangle2D grass_0 = {
        .min = {0,0},
        .max = {0,0}
    };
    static constexpr Syris::texture::Rectangle2D bush_with_berries = {
        .min = {0,0},
        .max = {0,0}
    };
    static constexpr Syris::texture::Rectangle2D bush_no_berries = {
        .min = {0,0},
        .max = {0,0}
    };
    static constexpr Syris::texture::Rectangle2D water_0 = {
        .min = {0,0},
        .max = {0,0}
    };
}; // namespace textures
