#pragma once
#include <vector>
#include <span>
#include "Syris/shader/Shader.hpp"

namespace Sandbox::triangle_scene{
    const char* path = "triangle_scene";
    /*
    using ShaderLayout = Syris::ShaderLayout<glm::vec3>;
    using ShaderLayoutTuple = std::tuple<glm::vec3>;
    Syris::Shader::CreateInfo get_shader_info(){
        const char * path = "triangle_scene";
        std::vector<std::string> names = {"uColor"};
        Syris::IShaderLayout* layout = static_cast<Syris::IShaderLayout *>(new ShaderLayout({names.begin(), names.end()}));
        Syris::Shader::CreateInfo info{
            .path = path,
            .layout = layout 
        };
        return info;
    }
    */
}