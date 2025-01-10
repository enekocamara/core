#pragma once
#include "Syris/layers/Layer.hpp"
#include "EngineState.hpp"
namespace GameEngine{
    class EnginePortal : public Syris::Layer{
    public:
        EnginePortal(EngineState& state);
        EnginePortal(const EnginePortal &ref) = delete;
        EnginePortal operator=(const EnginePortal &ref) = delete;

        bool on_event(Syris::Event *event) override;
        void on_update(const Syris::engine_time::Time &time);

    private:
        std::wstring get_user_path();

        std::wstring project_path;
        EngineState& m_state;
    };
}