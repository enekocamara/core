#pragma once
#include "Syris/Libs.h"
#include "glad/glad.h"

namespace shader{
    void compile_shader(int program_id,const char *vertex_shader_path, const char *fragment_shader_path);
};
