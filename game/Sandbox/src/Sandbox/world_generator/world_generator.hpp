#pragma once
#include <FastNoiseLite.h>
#include "Syris/log/Log.h"
#include "Syris/ecs/EntityManager.hpp"

#include "Sandbox/ecs/Bush.h"
#include "Sandbox/ecs/Tile.h"
#include "Sandbox/scene/sandbox/SandboxScene.hpp"

#include "Sandbox/scene/sandbox/GridLookUp.hpp"

namespace Sandbox::world_generator {
    template<glm::u32 x, glm::u32 y>
    std::array<entt::entity, x * y> make_bushes(glm::vec2 top_left, float padding, entt::registry& registry){
        auto texture = ecs::Bush::defaultTextureBundle();
        std::array<entt::entity, x * y> bushes;
        for (glm::u32 j = 0; j < y; j++){
            for (glm::u32 i = 0; i < x; i++){
//                bushes[j * y + i] = ecs::Bush::newBush(glm::vec2{i * padding + top_left.x, j * padding + top_left.y}, texture, registry);
            }
        }
        return bushes;
    }

    template<config::MapConfig map_config>
    using TileMap = std::array<ecs::CTile::TileType, map_config.num_tiles_x * map_config.num_tiles_y>;

    static glm::ivec2 wrapped_index(glm::ivec2 pos, glm::uvec2 dimmensions){
        if (pos.x < 0)
            pos.x = dimmensions.x - pos.x;
        else if (pos.x > dimmensions.x)
            pos.x -= dimmensions.x;
        if (pos.y < 0)
            pos.y = dimmensions.y - pos.y;
        else if (pos.y > dimmensions.y)
            pos.y -= dimmensions.y;
        return pos;
    }

    template<config::MapConfig map_config>
    inline ecs::CTile::TileType get_val(TileMap<map_config>& tile_map, glm::ivec2 pos){
        return tile_map[wrapped_index(pos, {map_config.num_tiles_x, map_config.num_tiles_y})];
    }

    template<config::MapConfig map_config>
    glm::u8 check_around(TileMap<map_config>& tiles,int x, int y){
        glm::u8 result = 0;
        result |= get_val<map_config>(tiles, {x-1, y+1});
    }
   
    static GridLookUp *generateGround(Syris::EntityManager& entity_manager, glm::uvec2 grid_dimmensions, glm::vec2 tile_dimmesions){
        //TileMap<map_config> tiles;
        FastNoiseLite noise;
        noise.SetSeed(1);
        noise.SetFractalOctaves(5);
        noise.SetFractalLacunarity(1);
        noise.SetFrequency(0.1);
        noise.SetFractalGain(0.5f);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
//        noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
//        noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Euclidean);

        auto texture = ecs::Tile::defaultTextureBundle();

        auto tile_gen = [noise](glm::uvec2 pos, Syris::EntityManager& entity_manager, uint32_t index) -> entt::entity
        {
            float noise_value = noise.GetNoise((float)pos.x, (float)pos.y);
            if (noise_value > 1.f || noise_value < -1.f)
            {
                Syris::Logger::client_info(std::format("noise value error: {}", noise_value).c_str());
            }
            if (noise_value < 0)
            {
                return ecs::Tile::newTile(glm::vec2(pos), texture::atlas::dirt_0, entity_manager, ecs::CTile::TileType::Water, index);
            }
            else
            {
                return ecs::Tile::newTile(glm::vec2(pos), texture::atlas::grass_0,entity_manager , ecs::CTile::TileType::Grass, index);
            }
        };

        GridLookUp::CreateInfo info{
            .grid_dimmension = grid_dimmensions,
            .tile_dimmensions = tile_dimmesions,
            .tile_gen = tile_gen,
            .entity_manager = entity_manager
        };
        return new GridLookUp(info);/*
        for (size_t y = 0; y < map_config.num_tiles_y; y++){
            for (size_t x = 0; x < map_config.num_tiles_x; x++){
                float noise_value = noise.GetNoise((float)x, (float)y);
                if (noise_value > 1.f || noise_value < -1.f){
                    Syris::Logger::client_info(std::format("noise value error: {}", noise_value).c_str());
                }
                std::cout << y * map_config.num_tiles_x + x << " ";
                if (noise_value < 0){
                    //std::cout << "DIRT\n";
                    texture.src = texture::atlas::dirt_0;
                    ecs::Tile::newTile(glm::vec2((float)x, (float)y), texture, registry, ecs::CTile::TileType::Water);
                }else{
                    //std::cout << "GRASS\n";
                    texture.src = texture::atlas::grass_0;
                    ecs::Tile::newTile(glm::vec2((float)x, (float)y), texture, registry, ecs::CTile::TileType::Grass);
                }
            }
        }*/
    }
}
