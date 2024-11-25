#include "SandboxLayers.hpp"
namespace Sandbox{
    /*
    SandboxLayer::SandboxLayer(SandboxData& data): m_data(data), m_debug_layer(m_data){

    }
    SandboxLayer::~SandboxLayer(){

    }

    void SandboxLayer::on_update(Syris::engine_time::Time& time){

    }
    bool SandboxLayer::on_event(Syris::Event* event){
        return m_debug_layer.on_event(event);
    }*/

    //render layer
    /*Functionality migrated to the scene
    SandboxRenderLayer::SandboxRenderLayer(SandboxData& data): m_data(data){
    }
    SandboxRenderLayer::~SandboxRenderLayer(){

    }

    void SandboxRenderLayer::on_update(){
        m_data.get_scene().onUpdate(m_data.get_camera().getCamera().get_view_projection_matrix());
        //!todo 
    }
    bool SandboxRenderLayer::on_event(Syris::Event* event){
        //!todo 
        return false;
    }
    */
    //debug layer
    /*
       SandboxDebugLayer::SandboxDebugLayer(SandboxData& data): m_data(data){

    }
    SandboxDebugLayer::~SandboxDebugLayer(){

    }

    void SandboxDebugLayer::on_update(Syris::engine_time::Time& time){
        //!todo 
    }
    bool SandboxDebugLayer::on_event(Syris::Event* event){
        //!todo 
        return false;
    }*/
}