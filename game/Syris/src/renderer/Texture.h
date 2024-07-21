#pragma once
#include "../Libs.h"
#include <array>
#include <deque>
#include <glad/glad.h>

class IEntityTexture;
namespace Syris::texture {
    struct Rectangle2D {
        glm::vec2 min;
        glm::vec2 max;
    };

    struct Texture2DBundle {
        Rectangle2D src;
        glm::fvec2 size;
        ///     Color color;
        float rotation;
    };

    struct Texture2DCreateBundle {
        unsigned int internal_format;
        unsigned int image_format;

        unsigned int wrap_s;
        unsigned int wrap_t;

        unsigned int filter_min;
        unsigned int filter_max;
    };

    struct Texture2D {
        Texture2D() { m_id = std::numeric_limits<GLuint>::max(); }
        Texture2D(std::string path, Texture2DCreateBundle createBundle);
        void bind();
        Texture2D(const Texture2D& ref){
            *this = ref;
        }
        Texture2D& operator=(const Texture2D& ref){
            this->m_id = ref.m_id;
            this->m_bundle = ref.m_bundle;
            this->m_width = ref.m_width;
            this->m_height = ref.m_height;
            this->m_tex_channels = ref.m_tex_channels;
            return *this;
        }
        ~Texture2D();

        Texture2DCreateBundle m_bundle;
        int m_width, m_height, m_tex_channels;
        GLuint m_id;
    };

    class TextureAtlas {
        public:
            TextureAtlas(const char* path);
            ~TextureAtlas();
            Texture2D getTexture() const {return m_texture;}
            void init();

        private:
            Texture2D m_texture;
            const char* m_path;
    };
}
