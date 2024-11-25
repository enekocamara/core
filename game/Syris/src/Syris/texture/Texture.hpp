#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class IEntityTexture;
namespace Syris {
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
    class Texture2D {
    public:
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

        Texture2D(std::vector<float> &data, glm::ivec2 size);

        Texture2D() { m_id = std::numeric_limits<GLuint>::max(); }
        Texture2D(const Texture2D &ref) = delete;
        /*Texture2D(const Texture2D& ref){
            *this = ref;
        }*/
        /*Texture2D& operator=(const Texture2D& ref){
            this->m_id = ref.m_id;
            this->m_bundle = ref.m_bundle;
            this->size = ref.size;
            this->m_tex_channels = ref.m_tex_channels;
            return *this;
        }*/
        ~Texture2D();

        void bind();
        GLuint get_id() const { return m_id; }
        glm::ivec2 get_size() const { return m_size; }

    private:
        Texture2DCreateBundle m_bundle;
        int m_tex_channels;
        glm::ivec2 m_size;
        GLuint m_id;
    };

    class TextureAtlas {
        public:
            TextureAtlas(const char* path);
            ~TextureAtlas();
            Texture2D& getTexture() {return m_texture;}


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
