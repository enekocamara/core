#include "Shader.hpp"

namespace Syris{
    //returns the name of the shader, it will be the name of the file without the .type
    std::string get_shader_name_from_path(std::string& path){
        size_t end = path.rfind('.');
        size_t begin = path.rfind('\\');
        return path.substr(begin, end);
    }
}

#ifdef RENDER_API_OPENGL
#include "OpenGLShader.hpp"
namespace Syris{
    Shader* Shader::create_shader(Shader::CreateInfo info){
        return new OpenGLShader(info);
    }
}

#endif