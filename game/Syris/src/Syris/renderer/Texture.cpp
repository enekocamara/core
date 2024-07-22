#include <format>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Syris/renderer/Texture.h"
#include "Syris/log/Log.h"

namespace Syris::texture{
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
        stbi_uc* pixels = stbi_load(path.c_str(), &m_width, &m_height, &m_tex_channels, STBI_rgb_alpha);
        if (!pixels){
            std::string error = std::format("Texture2D: failed to read image: path {}", path);
            Logger::core_error(error.c_str());
            throw std::runtime_error(error);
        }
        if (m_tex_channels != 4){
            std::cerr << "error reading texture channels" << m_tex_channels << '\n';
            exit(1);
        }
        int rowStride = m_width * 4; // 4 bytes per pixel (RGBA)
        std::vector<stbi_uc> flippedPixels(rowStride * m_height);

        for (int y = 0; y < m_height; ++y) {
            memcpy(
                    flippedPixels.data() + (m_height - 1 - y) * rowStride,
                    pixels + y * rowStride,
                    rowStride
                  );
        }

        glad_glGenTextures(1, &this->m_id);
        if (m_id == std::numeric_limits<GLuint>::max()){
            std::string error = std::format("Texture2D: failed to generate texture");
            Logger::core_error(error.c_str());
            throw std::runtime_error(error);
        }
        glBindTexture(GL_TEXTURE_2D, this->m_id);
        // set Texture wrap and filter modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, create_bundle.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, create_bundle.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, create_bundle.filter_min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, create_bundle.filter_max);

        glTexImage2D(GL_TEXTURE_2D, 0, create_bundle.internal_format, m_width,
                m_height, 0, create_bundle.image_format, GL_UNSIGNED_BYTE, flippedPixels.data());
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "texture id: " << m_id << '\n';
    }
    Texture2D::~Texture2D(){
    }
    void Texture2D::bind(){
        glBindTexture(GL_TEXTURE_2D, this->m_id);
    }
}
