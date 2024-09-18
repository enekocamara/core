#pragma once
#include "SandboxData.hpp"
#include "Syris/layers/Layer.hpp"

namespace Sandbox{
    class SandboxDebugLayer : public Syris::Layer{
        public:
        SandboxDebugLayer(SandboxData& data);
        ~SandboxDebugLayer();
        void on_update(Syris::engine_time::Time& time) override;
        bool on_event(Syris::Event *event) override;
        private:
            SandboxData& m_data;
    };/*
    class SandboxRenderLayer : public Syris::Layer{
        public:
        SandboxRenderLayer(SandboxData &data);
        ~SandboxRenderLayer();
        void on_update() override;
        bool on_event(Syris::Event *event) override;
        private:
            SandboxData& m_data;
    };*/
    class SandboxLayer: public Syris::Layer{
        public:
            SandboxLayer(SandboxData& data);
            ~SandboxLayer();
            void on_update(Syris::engine_time::Time& time);
            bool on_event(Syris::Event* event);
        private:
            SandboxData& m_data;
            SandboxDebugLayer m_debug_layer;
            //SandboxRenderLayer m_render_layer;
            //ui layer?
    };
}