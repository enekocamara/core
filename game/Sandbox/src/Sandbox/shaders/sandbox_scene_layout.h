#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"
#include "Syris/renderer/Texture.h"

namespace Sandbox::sandbox_scene{
    using ShaderLayout = Syris::ShaderLayout<glm::mat4, Syris::texture::Texture2D>;
    using ShaderLayoutTuple = std::tuple<glm::mat4, Syris::texture::Texture2D>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "sandbox_scene";
        std::vector<std::string> names = {"ViewProjection", "texture1"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
}