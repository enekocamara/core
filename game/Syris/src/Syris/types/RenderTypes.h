#pragma once
#include "Type.h"
#include "Syris/include/imgui.h"
#include "Syris/utils/Breakpoint.h"

namespace Syris::type{

    inline void render_type(RealType<Type::i32> *data){
        ImGui::Text("[%d]", *data);
    }
    inline void render_type(RealType<Type::f32> *data){
        ImGui::Text("[%f]", *data);
    }
    inline void render_type(RealType<Type::u32> *data){
        ImGui::Text("[%u]", *data);
    }
    
    inline void render_type(RealType<Type::ivec2> *data){
        ImGui::Text("[%d, %d]", data->x , data->y);
    }
    inline void render_type(RealType<Type::vec2> *data){
        ImGui::Text("[%f, %f]", data->x , data->y);
    }
    inline void render_type(RealType<Type::uvec2> *data){
        ImGui::Text("[%u, %u]", data->x , data->y);
    }

    inline void render_type(RealType<Type::ivec3> *data){
        ImGui::Text("[%d, %d, %d]", data->x , data->y, data->z);
    }
    inline void render_type(RealType<Type::vec3> *data){
        ImGui::Text("[%f, %f, %f]", data->x , data->y, data->z);
    }
    inline void render_type(RealType<Type::uvec3> *data){
        ImGui::Text("[%u, %u, %u]", data->x , data->y, data->z);
    }

    inline void render_type(RealType<Type::ivec4> *data){
        ImGui::Text("[%d, %d, %d, %d]", data->x , data->y, data->z, data->w);
    }
    inline void render_type(RealType<Type::vec4> *data){
        ImGui::Text("[%f, %f, %f, %f]", data->x , data->y, data->z, data->w);
    }
    inline void render_type(RealType<Type::uvec4> *data){
        ImGui::Text("[%u, %u, %u, %u]", data->x , data->y, data->z, data->w);
    }
    inline void render_type(RealType<Type::mat4> *data){

        ImGui::Text("[%f, %f, %f, %f", (*data)[0][0], (*data)[0][1],(*data)[0][2],(*data)[0][3]);
        ImGui::Text(" %f, %f, %f, %f", (*data)[1][0], (*data)[1][1],(*data)[1][2],(*data)[1][3]);
        ImGui::Text(" %f, %f, %f, %f", (*data)[2][0], (*data)[2][1],(*data)[2][2],(*data)[2][3]);
        ImGui::Text(" %f, %f, %f, %f]", (*data)[3][0], (*data)[3][1],(*data)[3][2],(*data)[3][3]);
    }

    inline void render_type(Type type, void *data){
        switch (type){
        case Type::f32:
            render_type(static_cast<RealType<Type::f32> *>(data));
            break;
        case Type::i32:
            render_type(static_cast<RealType<Type::i32> *>(data));
            break;
        case Type::u32:
            render_type(static_cast<RealType<Type::i32> *>(data));
            break;
        case Type::vec2:
            render_type(static_cast<RealType<Type::vec2> *>(data));
            break;
        case Type::uvec2:
            render_type(static_cast<RealType<Type::uvec2> *>(data));
            break;
        case Type::ivec2:
            render_type(static_cast<RealType<Type::ivec2> *>(data));
            break;
        case Type::vec3:
            render_type(static_cast<RealType<Type::vec3> *>(data));
            break;
        case Type::uvec3:
            render_type(static_cast<RealType<Type::uvec3> *>(data));
            break;
        case Type::ivec3:
            render_type(static_cast<RealType<Type::ivec3> *>(data));
            break;
        case Type::vec4:
            render_type(static_cast<RealType<Type::vec4> *>(data));
            break;
        case Type::uvec4:
            render_type(static_cast<RealType<Type::uvec4> *>(data));
            break;
        case Type::ivec4:
            render_type(static_cast<RealType<Type::ivec4> *>(data));
            break;
        case Type::mat4:
            render_type(static_cast<RealType<Type::mat4> *>(data));
            break;
        case Type::texture2D:
            BREAK_POINT("TODO!");
            break;
        }
    }
}