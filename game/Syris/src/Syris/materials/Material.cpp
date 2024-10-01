#include "Material.hpp"



#ifdef RENDER_API_OPENGL
//opengl implementation of Material

#include "OpenGLMaterial.hpp"
namespace Syris{
    Material* Material::create_material(Material::CreateInfo& info){
        return new OpenGLMaterial(info);
    }
}

#endif