#pragma once
#include "Layer.hpp"
#include <unordered_map>
#include <span>
#include "Syris/events/Event.hpp"
#include <boost/serialization/strong_typedef.hpp>
namespace Syris{
    using LayerID = uint32_t;
    class LayerManager
    {
    public:
    
        struct CreateInfo
        {
            std::span<Layer *> layers;
        };
        //LayerManager(CreateInfo &info);
        LayerManager();
        ~LayerManager(){};

        //LayerManager() = delete;
        LayerManager(const LayerManager &ref) = delete;
        LayerManager &operator=(const LayerManager &ref) = delete;

        bool on_event(Event *event);
        void on_update(engine_time::Time& time);
        LayerID push_layer(Layer* layer);
        void pop_layer(LayerID layer);
        LayerID replace_layer(Layer* layer, LayerID id_to_replace);

    private:
        // owning pointers will be freed at destruction
        LayerID m_currentID = LayerID(1);
        std::vector<LayerID> m_layers_order;
        std::unordered_map<LayerID, Layer*> m_layers_map;
    };
}