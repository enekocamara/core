#pragma once
#include "FrameTexture2D.hpp"
#include "FastNoiseLite.h"
#include "Syris/utils/noise/NoiseGenerator.hpp"

/*
    Code is mostly from fast noise lite web example:
    https://github.com/Auburn/FastNoiseLite/blob/master/WebPreviewApp/main.cpp 
*/

namespace Syris{
    class TextureEditor{
    public:
        TextureEditor(FrameTexture2D& texture, std::string name):m_frame_texture(texture), m_name(name){};
        bool render();
        void update_texture(bool new_preview);
    private:
        FrameTexture2D &m_frame_texture;
        std::string m_name;
        std::vector<glm::vec3> m_data;
        FastNoiseLite m_noise;
        FastNoiseLite m_warp;
        Syris::NoisePattern m_pattern;
    };
}