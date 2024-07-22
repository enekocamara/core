#pragma once
#include "Syris/Libs.h"
#include "Syris/renderer/renderer2D/Renderer.h"
namespace scene{
    class Scene{
        public:
            Scene();
            virtual ~Scene(){};

           // virtual void draw_frame(ge::Renderer2D& renderer) = 0;
        private:
    };
}
