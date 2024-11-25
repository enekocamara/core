#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"
/*
namespace Sandbox::simple_scene{
    using ShaderLayout = Syris::ShaderLayout<glm::mat4>;
    using ShaderLayoutTuple = std::tuple<glm::mat4>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "simple_scene";
        std::vector<std::string> names = {"ViewProjection"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
}
*/