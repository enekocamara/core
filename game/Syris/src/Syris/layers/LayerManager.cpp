#include "LayerManager.hpp"

namespace Syris{
    LayerManager::LayerManager(CreateInfo &info){
        m_layers.insert(m_layers.end(), info.layers.begin(), info.layers.end());
    }
    void LayerManager::on_update(){
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); it++){
            (*it)->on_update();
        }
    }
    bool LayerManager::on_event(Event* event){
        for (auto it = m_layers.begin(); it != m_layers.end(); it++){
            if ((*it)->on_event(event))
                return true;
        }
        return false;
    }
    void LayerManager::push_layer(Layer* layer){
        m_layers.push_back(layer);
    }
}