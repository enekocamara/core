#pragma once
#include "Syris/Libs.h"
#include "Syris/renderer/renderer2D/Renderer.h"
#include "Syris/layers/Layer.hpp"
namespace Syris{
    class Scene : public Layer{
        public:
            Scene();
            virtual ~Scene(){};
             
           // virtual void draw_frame(ge::Renderer2D& renderer) = 0;
        private:
    };
}
