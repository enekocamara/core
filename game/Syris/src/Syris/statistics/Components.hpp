#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include "Types.hpp"

namespace Syris::statistics{
    struct CGPUMem{
        uint64_t size;
    };
    struct CGPUMemRef{
        const uint64_t& size;
    };
    struct CCPUMem{
        uint64_t size;
    };
    struct CCPUMemRef{
        const uint64_t& size;
    };

    struct CScene{
        std::string scene_name;
    };
    struct CMaterial{
        std::string material_name;
    };
    struct CVertexBuffer{
    };
    struct CSubBuffer{
    };
    struct Identified{
        std::string indentifier;
    };
    struct CStatisticsRoot{};
    struct CMaterialManager{};
    struct CSceneManager{};
    struct CRender{
        std::function<void(entt::entity, entt::registry&)> render;
    };
    struct CChilds{
        //add_childs make it so that u can put a custom function that runs checks to ensure u implement necesary things
        std::vector<StatisticModID> childs;
    };

    inline void render_childs(entt::entity entity, entt::registry& registry){ 
        for (auto child : registry.get<statistics::CChilds>(entity).childs){
            registry.get<statistics::CRender>(child).render(child, registry);
        }
    }
}