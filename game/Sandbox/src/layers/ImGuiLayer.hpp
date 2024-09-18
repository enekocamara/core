#include "Syris/layers/Layer.hpp"

#include "Syris/include/imgui.h"


namespace Sandbox{
    class ImGuiLayer : public Syris::Layer{
        public:
            ImGuiLayer();
            ~ImGuiLayer();
            void on_update(Syris::engine_time::Time& time) override;
            bool on_event(Syris::Event *event) override;
    };
}