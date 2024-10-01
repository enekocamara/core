#pragma once
#include <array>
#include <glm/glm.hpp>
#include "../texture/SandboxTexture.hpp"
#include <iostream>

namespace Sandbox{
    struct TileVertices{
        std::array<float, 8> vertices;
        TileVertices(){
            vertices = {
                // pos     //uv texture
                -1.0f, 1.0f,//  0.0f, 1.0f,// top-left
                1.0f,  1.0f,//  1.0f, 1.0f,// top-right
                1.0f,  -1.0f,// 1.0f, 0.0f,// bottom-right
                -1.0f, -1.0f,// 0.0f, 0.0f,// bottom-left
            };
        }
        std::pair<glm::vec2, glm::vec2> get_min_max(){
            return {{vertices[2], vertices[6]},{vertices[11],vertices[3]}}; 
        }

    };
    struct TileIndices{
        std::array<uint32_t, 6> vertices;
        TileIndices(){
            vertices = {
                0,1,2,2,3,0
            };
        }
    };
    struct TileInstancedData{
        glm::vec4 tex_coord = {texture::atlas::grass_0.min, texture::atlas::grass_0.max};
        glm::mat4 translation = glm::mat4(1.f);
    };

    struct TileVerticesTriangle{
        std::array<float, 6> vertices;
        TileVerticesTriangle(int a){
            std::cout << "tile  vertices\n";
            this->vertices = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.0f, 0.5f
            };
        }
    };
}