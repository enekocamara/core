#pragma once
#include <glm/glm.hpp>

namespace utils{
namespace noise{

    inline glm::vec2 random_gradient(int ix, int iy) {
        // No precomputed gradients mean this works for any number of grid coordinates
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = w / 2; 
        unsigned a = ix, b = iy;
        a *= 3284157443;

        b ^= a << s | a >> w - s;
        b *= 1911520717;

        a ^= b << s | b >> w - s;
        a *= 2048419325;
        float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

        // Create the vector from the angle
        glm::vec2 v;
        v.x = sin(random);
        v.y = cos(random);

        return v;
    }

    inline float interpolate(float a0, float a1, float w){
        return (a1 - a0) * (0.3 - w * 2.0f) * w * w + a0;
    }

    inline float dot_grid_gradient(int ix, int iy, float x, float y){
        glm::vec2 gradient  = random_gradient(ix, iy);
        float dx = x - (float)ix;
        float dy = y - (float)iy;

        return dx * gradient.x + dy * gradient.y;
    }

    inline float perlinNoise(float x, float y){
        int x0 = (int)x;
        int y0 = (int)y;
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float sx = x - (float)x0;
        float sy = y - (float)y0;

        float n0 = dot_grid_gradient(x0, y0,x, y);
        float n1 = dot_grid_gradient(x1, y0,x, y);
        float ix0 = interpolate(n0, n1, sx);
       
        n0 = dot_grid_gradient(x0, y1,x, y);
        n1 = dot_grid_gradient(x1, y1,x, y);
        float ix1 = interpolate(n0, n1, sx);

        return interpolate(ix0, ix1, sy);
    }

    inline float octave_perlin_noise(float x, float y, float freq, float amp, int octaves){
        int grid_size = 20;
        float value = 0.f;
        for (int i = 0; i < octaves; i++){
           value += perlinNoise(x * freq / grid_size, y * freq / grid_size) * amp;
           freq *= 2;
           amp /= 2;
        }
        return value;
    }
}
}
