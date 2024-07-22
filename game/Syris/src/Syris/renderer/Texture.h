#pragma once
#include <glad/glad.h>

#include "Syris/Libs.h"

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

    /*
     *@Brief 2D texture wrapper
     *
     * 2D texture wrapper, loads from path, binds with
     * graphics API
     */
    struct Texture2D {
        /**
         * @Brief Loads a rgba image to a Texture2D
         *
         * Loads a rgba image to a Texture2D using stdbi library
         * and binds it to opengl as a glTexImage2D
         *
         * @param path to the image
         * @param Texture2DCreateBundle all the flags for opengl image
         *
         * @throws std::runtime_error if std_load fails to load
         * or glad_glGenTextures fails
         */ 
        Texture2D(std::string path, Texture2DCreateBundle createBundle);


        Texture2D() { m_id = std::numeric_limits<GLuint>::max(); }
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


            /*
            * @throws std::runtime_error if std_load fails to load
            * or glad_glGenTextures fails
            **/
            void init();

        private:
            Texture2D m_texture;
            const char* m_path;
    };
}
