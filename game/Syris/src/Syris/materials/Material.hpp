#pragma once

namespace Syris{
    class Material{
        public:
            virtual ~Material(){}
            virtual void set_data(void *data) = 0; 
    };
}