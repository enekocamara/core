#include <format>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Syris/texture/Texture.hpp"
#include "Syris/log/Log.h"

namespace Syris{
    TextureAtlas::TextureAtlas(const char* path) : m_path(path) {}
    TextureAtlas::~TextureAtlas() {}

    void TextureAtlas::init() {
        Texture2DCreateBundle bundle  = Texture2DCreateBundle{
            .internal_format = GL_RGBA,
                .image_format = GL_RGBA,
                .wrap_s = GL_REPEAT,
                .wrap_t = GL_REPEAT,
                .filter_min = GL_LINEAR,
                .filter_max = GL_LINEAR
        };
        m_texture = Texture2D(m_path, bundle);
    }

    Texture2D::Texture2D(std::string path, Texture2DCreateBundle create_bundle) : m_bundle(create_bundle) {
        m_id = std::numeric_limits<GLuint>::max();
        stbi_uc* pixels = stbi_load(path.c_str(), &m_size.x, &m_size.y, &m_tex_channels, STBI_rgb_alpha);
        if (!pixels){
            std::string error = std::format("Texture2D: failed to read image: path {}", path);
            CORE_ERROR(error.c_str());
            throw std::runtime_error(error);
        }
        if (m_tex_channels != 4){
            CORE_ERROR(std::format("Error reading texture channels {}", m_tex_channels));
            exit(1);
        }
        int rowStride = m_size.x * 4; // 4 bytes per pixel (RGBA)
        std::vector<stbi_uc> flippedPixels(rowStride * m_size.y);

        for (int y = 0; y < m_size.y; ++y) {
            memcpy(
                    flippedPixels.data() + (m_size.y - 1 - y) * rowStride,
                    pixels + y * rowStride,
                    rowStride
                  );
        }

        glad_glGenTextures(1, &this->m_id);
        if (m_id == std::numeric_limits<GLuint>::max()){
            std::string error = std::format("Texture2D: failed to generate texture");
            CORE_ERROR(error);
            throw std::runtime_error(error);
        }
        glBindTexture(GL_TEXTURE_2D, this->m_id);
        // set Texture wrap and filter modes

        glTexImage2D(GL_TEXTURE_2D, 0, create_bundle.internal_format, m_size.x,
                m_size.y, 0, create_bundle.image_format, GL_UNSIGNED_BYTE, flippedPixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, create_bundle.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, create_bundle.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, create_bundle.filter_min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, create_bundle.filter_max);
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
        CORE_INFO(std::format("Texture id: {}", m_id));
    }
    Texture2D::Texture2D(std::vector<float>& data, glm::ivec2 size){
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, data.data());
        // Set texture filtering for minification and magnification to GL_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // (Optional) Set wrap modes to control behavior for UVs outside [0, 1]
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_size = size;
    }
    Texture2D::~Texture2D(){
    }
    void Texture2D::bind(){
        glBindTexture(GL_TEXTURE_2D, this->m_id);
    }
}
