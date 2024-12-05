#pragma once

#include <vector>
#include <functional>

#include <entt.hpp>
#include <glm/glm.hpp>

namespace Syris{
    template<typename T>
    class GridLookUp{
    public:
        struct CreateInfo{
            glm::uvec2 grid_dimmension;
            std::vector<T> &&data;
        };
        GridLookUp(CreateInfo &&info)
            : m_data(std::move(info.data)),
              m_grid_dimmensions(info.grid_dimmension)
        {}


        GridLookUp(glm::uvec2 dimmensions)//requires(std::is_default_constructible<T>)
            : m_data(dimmensions.x * dimmensions.y),
              m_grid_dimmensions(dimmensions)
        {}

        ~GridLookUp() {};
        GridLookUp() = delete;
        GridLookUp(const GridLookUp &ref) = delete;

        T& operator[](std::size_t index){
            return m_data[index];
        }

        T& operator[](glm::uvec2 pos){
            return m_data[pos.y * m_grid_dimmensions.x + pos.x];
        }

        T& get(std::size_t index){
            return m_data[index];
        }

        T& get(glm::uvec2 pos){
            return m_data[pos.y * m_grid_dimmensions.x + pos.x];
        }

        std::size_t get_index(glm::uvec2 pos){
            return pos.y * m_grid_dimmensions.x + pos.x;
        }

    private:
        std::vector<T> m_data;
        glm::uvec2 m_grid_dimmensions;
    };
}