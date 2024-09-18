#pragma once
#include "Material.hpp"
#include "OpenGLShader.hpp"



//i want to combine updating data with shaders
//render buffer is a implementation of a system i want to make

namespace Syris{
    class OpenGLMaterial : public Material{
        public:
            struct CreateInfo{
                OpenGLShader::CreateInfo shader_info;
            };
            OpenGLMaterial();
            ~OpenGLMaterial(){}
            void set_data(void *data)override;
        private:
            OpenGLShader* shader; 
            
    };
}