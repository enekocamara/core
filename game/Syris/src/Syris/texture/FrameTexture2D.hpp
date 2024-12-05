#pragma once

#include "Texture.hpp"
#include "Syris/shader/ShaderManger.hpp"
#include "Syris/renderer/FrameBuffer.hpp"
#include "Syris/renderer/Renderer.hpp"
namespace Syris{
    class FrameTexture2D{
        public:
            struct CreateInfo{
                Texture2D* texture;
                ShaderManager& shader_manager;
                ShaderManager::ShaderID shader_id;
                glm::ivec2 size;
            };
            FrameTexture2D(CreateInfo info);
            void render();
            glm::ivec2 get_size() const{return m_texture->get_size();}
            GLuint get_texture_id()const{
                return m_frame_buffer.get_texture_id();
            }
            Texture2D* get_texture(){return m_texture;}
        private:
            Texture2D* m_texture;
            ShaderManager::ShaderID m_shader_id;
            ShaderManager& m_shader_manager;
            FrameBuffer m_frame_buffer;
            //glm::ivec2 m_size;
    };
}