#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Syris/utils/Breakpoint.h"
#include "Syris/utils/Assert.h"

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

    template <typename T>
    concept CompatibleType =
        std::is_same_v<T, float> ||
        std::is_same_v<T, glm::vec2> ||
        std::is_same_v<T, glm::vec3> ||
        std::is_same_v<T, glm::vec4>;
        

    template<typename T, typename U>
    concept CompatibleExternal =
        std::is_layout_compatible<T, U>::value;

    /*
     *@Brief 2D texture wrapper
     *
     * 2D texture wrapper, loads from path, binds with
     * graphics API
     */
    class Texture2D
    {
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

        template<CompatibleType T>
        Texture2D(std::vector<T> &data, glm::ivec2 size, Texture2DCreateBundle create_bundle) : m_bundle(create_bundle) {
            glGenTextures(1, &m_id);
            glBindTexture(GL_TEXTURE_2D, m_id);
            if constexpr (std::is_layout_compatible_v<T, float>){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, data.data());
                m_tex_channels = 1;
            }
            else if constexpr(std::is_layout_compatible_v<T, glm::vec2>){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size.x, size.y, 0, GL_RG, GL_FLOAT, data.data());
                m_tex_channels = 2;
            }
            else if constexpr(std::is_layout_compatible_v<T, glm::vec3>){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x, size.y, 0, GL_RGB, GL_FLOAT, data.data());
                m_tex_channels = 3;
            }
            else if constexpr(std::is_layout_compatible_v<T, glm::vec4>){
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, data.data());
                m_tex_channels = 4;
            }
            else
                BREAK_POINT("Should never reach");
            m_gl_type = tex_chann_to_openGL_type(m_tex_channels); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, create_bundle.wrap_s);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, create_bundle.wrap_t);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, create_bundle.filter_min);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, create_bundle.filter_max);

            /*// Set texture filtering for minification and magnification to GL_NEAREST
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // (Optional) Set wrap modes to control behavior for UVs outside [0, 1]
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
            glBindTexture(GL_TEXTURE_2D, 0);
            m_size = size;
        }

        template<CompatibleType T>
        void update(std::vector<T> &data){
            glBindTexture(GL_TEXTURE_2D, m_id);
            
            GLint xOffset = 0;
            GLint yOffset = 0;
            GLsizei width = m_size.x;
            GLsizei height = m_size.y;

            
            if constexpr (std::is_same_v<T, float>)
                ASSERT(m_gl_type == GL_RED, "channel missmatch");
            else if constexpr(std::is_same_v<T, glm::vec2>)
                ASSERT(m_gl_type == GL_RG, "channel missmatch");
            else if constexpr(std::is_same_v<T, glm::vec3>)
                ASSERT(m_gl_type == GL_RGB, "channel missmatch");
            else if constexpr(std::is_same_v<T, glm::vec4>)
                ASSERT(m_gl_type == GL_RGBA, "channel missmatch");
            else
                BREAK_POINT("Should never reach");

            ASSERT(data.size() * sizeof(T) / sizeof(float) == m_size.x * m_size.y * m_tex_channels, "passed data's size is incorrect");

            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, m_gl_type, GL_FLOAT, data.data());
            glBindTexture(GL_TEXTURE_2D, 0);
        }


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
        GLuint tex_chann_to_openGL_type(int)const;

        Texture2DCreateBundle m_bundle;
        int m_tex_channels;
        glm::ivec2 m_size;
        GLuint m_gl_type;
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
