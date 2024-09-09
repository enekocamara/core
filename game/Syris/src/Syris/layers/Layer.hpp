#pragma once
#include "Syris/events/Event.hpp"

namespace Syris
{
    class Layer
    {
    public:
        Layer() {};
        virtual ~Layer() {};
        virtual void on_update() = 0;
        virtual bool on_event(Event *event) = 0;
    };
}