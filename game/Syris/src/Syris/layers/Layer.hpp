#pragma once
#include "Syris/events/Event.hpp"
#include "Syris/utils/EngineTime.hpp"
namespace Syris
{
    class Layer
    {
    public:
        Layer() {};
        virtual ~Layer() {};
        virtual void on_update(engine_time::Time& time) = 0;
        virtual bool on_event(Event *event) = 0;
    };
}