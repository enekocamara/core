#pragma once

#include "Texture.hpp"
#include "Syris/shader/ShaderManger.hpp"
#include "Syris/renderer/FrameBuffer.hpp"
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
            GLuint get_texture_id()const{
                return m_frame_buffer.get_texture_id();
            }
        private:
            Texture2D* m_texture;
            ShaderManager::ShaderID m_shader_id;
            ShaderManager& m_shader_manager;
            FrameBuffer m_frame_buffer;
            glm::ivec2 m_size;
    };
}