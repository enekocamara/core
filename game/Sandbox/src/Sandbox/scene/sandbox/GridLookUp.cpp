#include "GridLookUp.hpp"

namespace Sandbox{
    GridLookUp::GridLookUp(CreateInfo& info):
        m_entities_grid(info.grid_dimmension.x * info.grid_dimmension.y),
        m_grid_dimmensions(info.grid_dimmension),
        m_tile_dimmensions(info.tile_dimmensions){
        uint32_t width = m_grid_dimmensions.x;
        
        /* not worth it
        auto coord = [width](uint32_t x, uint32_t y){
            return y * width + x; 
        };
        */
        for (uint32_t y = 0; y < info.grid_dimmension.y; y++){
            for (uint32_t x = 0; x < info.grid_dimmension.x; x++){
                m_entities_grid[y * width + x] = info.tile_gen(glm::uvec2(x,y), info.entity_manager, y * width + x);   
            }
        }
    }
    GridLookUp::~GridLookUp(){

    }
    entt::entity GridLookUp::get_entity(glm::vec2 pos){
        //todo
        return m_entities_grid[-1];
    }
    entt::entity GridLookUp::get_entity(uint32_t index){
        //todo
        return m_entities_grid[index];
        
    }
}