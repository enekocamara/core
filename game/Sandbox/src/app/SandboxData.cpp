#include "SandboxData.hpp"
#include "../scene/TriangleScene.hpp"
namespace Sandbox{
    SandboxData::SandboxData(CreateInfo info)
        : m_registry(info.registry),
          m_scene(new SimpleScene({m_registry, info.atlas_path, info.graphics_context, info.camera_info})),
          m_graphics_context(info.graphics_context){
        m_graphics_context.get_layer_manager().push_layer(m_scene);
    }
    SandboxData::~SandboxData(){

    }
}