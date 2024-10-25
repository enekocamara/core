#pragma once

#include "Material.hpp"
#include "OpenGLMaterial.hpp"

namespace Syris {
    template <class T>
    Material* create_material(Material::CreateInfo& info)
    {
        return new OpenGLMaterial<T>(info);
    }
}