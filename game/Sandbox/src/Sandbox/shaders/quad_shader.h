#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"
#include "Syris/renderer/Texture.h"

namespace Sandbox::quad_shader{
    using ShaderLayout = Syris::ShaderLayout<>;
    using ShaderLayoutTuple = std::tuple<>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "quad";
        std::vector<std::string> names = {};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
}