#pragma once
#include <vector>
#include <string>

/*
    Plains mild temperature and mild humidity;
    Arid plain mild temperature and low humidity;
    Tundra low temperature and mild humidity;
*/
namespace Sandbox {
    struct Range{
        float min;
        float max;
    };
    class Biome{
        public:
            struct CreateInfo{
                std::string name;
            };
            Biome(CreateInfo info);
            ~Biome();
        private:
            std::string name;
    };
    struct Slice{
        Biome biome;
        std::vector<Slice> slices;
    };
    class Biomes{
        public:
            Biomes(Biome default_biome);

        private:
            Slice m_root;
    };
}