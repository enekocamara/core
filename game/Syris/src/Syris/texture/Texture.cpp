#include <format>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Syris/texture/Texture.hpp"
#include "Syris/log/Log.h"
#include "Syris/utils/Breakpoint.h"

namespace Syris{
    TextureAtlas::TextureAtlas(const char* path) : m_path(path) {}
    TextureAtlas::~TextureAtlas() {}

    void TextureAtlas::init() {
        Texture2DCreateBundle bundle  = Texture2DCreateBundle{
            .internal_format = GL_RGBA,
                .image_format = GL_RGBA,
                .wrap_s = GL_REPEAT,
                .wrap_t = GL_REPEAT,
                .filter_min = GL_NEAREST,
                .filter_max = GL_NEAREST
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
        if (m_tex_channels != 4)
            BREAK_POINT(std::format("Error reading texture channels {}", m_tex_channels));
        
        m_gl_type = tex_chann_to_openGL_type(m_tex_channels); 
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
    Texture2D::~Texture2D(){
    }
    void Texture2D::bind(){
        glBindTexture(GL_TEXTURE_2D, this->m_id);
    }
    GLuint Texture2D::tex_chann_to_openGL_type(int channel_count)const{
        switch (channel_count){
            case 1:
                return GL_RED;
            case 2: 
                return GL_RG;
            case 3: 
                return GL_RGB;
            case 4:
                return GL_RGBA;
            default:
                BREAK_POINT("Invalid channel count"); 
        }
    }
}
