#include "LayerManager.hpp"
#include <algorithm>
#include "Syris/log/Log.h"
namespace Syris{
    LayerManager::LayerManager(){
        /*m_layers_order.push_back(m_currentID);
        m_currentID = LayerID(m_currentID + 1);
        m_layers_map.insert(m_layers.end(), info.layers.begin(), info.layers.end());*/
    }
    void LayerManager::on_update(const engine_time::Time& time){
        for (auto it = m_layers_order.rbegin(); it != m_layers_order.rend(); it++){
             m_layers_map[*it]->on_update(time);
        }
    }
    bool LayerManager::on_event(Event* event){
        for (auto id : m_layers_order){
            if (m_layers_map[id]->on_event(event))
                return true;
        }
        return false;
    }
    LayerID LayerManager::push_layer(Layer* layer){
        LayerID id = m_currentID;
        m_layers_order.push_back(id);
        m_currentID = LayerID(m_currentID + 1);
        m_layers_map[id] = layer;
        return id;
    }

    void LayerManager::pop_layer(LayerID id){
        auto it = std::find(m_layers_order.begin(), m_layers_order.end(), id);
        if (it != m_layers_order.end()){
            m_layers_map.erase(id);
            m_layers_order.erase(it);
        }
    }

    LayerID LayerManager::replace_layer(Layer* layer, LayerID id_to_replace){
        auto it = std::find(m_layers_order.begin(), m_layers_order.end(), id_to_replace);
        if (it != m_layers_order.end()){
            LayerID id = m_currentID;
            m_currentID = LayerID(m_currentID + 1);
            *it = id; 
            m_layers_map.erase(id_to_replace);
            m_layers_map[id] = layer;
            return id;
        }else{
            CORE_ERROR("failed to replace layer");
            exit(1);
        }
    }
}