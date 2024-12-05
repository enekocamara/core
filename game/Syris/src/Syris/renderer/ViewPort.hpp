#pragma once
#include <glm/glm.hpp>
#include "Syris/include/OpenGL.h"

namespace Syris{
    class ViewPort{
        public:
        ViewPort(glm::ivec2 dimmensions):m_dimmensions(dimmensions){};
        void set(){glViewport(0,0,m_dimmensions.x, m_dimmensions.y);}
        glm::ivec2 get_dimmesions()const{return m_dimmensions;}
        void set_dimmensions(glm::ivec2 dimmensions){m_dimmensions = dimmensions;}
        private:
        glm::ivec2 m_dimmensions;
    };
}