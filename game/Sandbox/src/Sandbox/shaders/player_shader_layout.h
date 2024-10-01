#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"
#include "Syris/renderer/Texture.h"

namespace Sandbox::player_shader{
    using ShaderLayout = Syris::ShaderLayout<glm::mat4, glm::mat4, Syris::texture::Texture2D>;
    using ShaderLayoutTuple = std::tuple<glm::mat4, glm::mat4, Syris::texture::Texture2D>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "player";
        std::vector<std::string> names = {"ViewProjection", "model", "texture1"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
}