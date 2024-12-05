#pragma once
#include <glm/glm.hpp>

#include "Syris/include/OpenGL.h"
namespace Syris{
    class FrameBuffer{
        public:
            FrameBuffer(glm::ivec2 size);
            FrameBuffer(GLuint framebbufer_id, glm::ivec2 size);//for main framebuffer abstraction
            ~FrameBuffer(){};

            void bind(){
                glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            };
            GLuint get_texture_id()const{
                return m_fb_texture;
            }
            glm::ivec2 get_size()const{return m_size;}
        private:
            GLuint m_framebuffer;
            GLuint m_fb_texture;
            glm::ivec2 m_size;
    };
}