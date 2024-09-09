#pragma once
#include "Layer.hpp"
#include <unordered_map>
#include <span>
#include "Syris/events/Event.hpp"
namespace Syris{
    class LayerManager
    {
    public:
        struct CreateInfo
        {
            std::span<Layer *> layers;
        };
        LayerManager(CreateInfo &info);
        ~LayerManager(){};

        LayerManager() = delete;
        LayerManager(const LayerManager &ref) = delete;
        LayerManager &operator=(const LayerManager &ref) = delete;

        bool on_event(Event *event);
        void on_update();
        void push_layer(Layer* layer);

    private:
        // owning pointers will be freed at destruction
        std::vector<Layer *> m_layers;
    };
}