#pragma once

namespace Sandbox{
    struct TileInstanceMaterial
    {
        float height = 0;
        float humidity = 0;
        float heat = 0;
    };
    struct TileInstancedData{
        glm::mat4 trans = glm::mat4(1);
        TileInstanceMaterial mat;
    };
}