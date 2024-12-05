#include "FrameBuffer.hpp"

namespace Syris{
    FrameBuffer::FrameBuffer(glm::ivec2 size):m_size(size){
        glGenTextures(1, &m_fb_texture);
        glBindTexture(GL_TEXTURE_2D, m_fb_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fb_texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER,m_framebuffer);
        glViewport(0, 0, m_size.x, m_size.y);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } 
    FrameBuffer::FrameBuffer(GLuint framebbufer_id, glm::ivec2 size){
        m_framebuffer = framebbufer_id;
        m_fb_texture = -1;
        m_size = size; 
    }
}
