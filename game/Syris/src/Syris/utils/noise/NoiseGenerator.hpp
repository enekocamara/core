#pragma once
#include "FastNoiseLite.h"
#include "json.hpp"
#include <functional>
#include <glm/glm.hpp>

namespace Syris{

    struct WarpedNoise{
        FastNoiseLite noise;
        FastNoiseLite warp;
    };

    struct NoisePattern{
        WarpedNoise get_noise(){
            FastNoiseLite noise;
            FastNoiseLite warp;

            noise.SetNoiseType(m_noise_type);
            noise.SetSeed(m_seed);
            noise.SetFrequency(m_frequency);

            noise.SetFractalType(m_fractal_type);
            noise.SetFractalOctaves(m_fractal_octaves);
            noise.SetFractalLacunarity(m_fractal_lacunarity);
            noise.SetFractalGain(m_fractal_gain);
            noise.SetFractalWeightedStrength(m_fractal_weighted_strength);
            noise.SetFractalPingPongStrength(m_fractal_ping_pong_strength);

            //todo cellular type
            noise.SetCellularDistanceFunction(m_cellular_distance_function);
            noise.SetCellularReturnType(m_cellular_return_type);
            noise.SetCellularJitter(m_cellular_jitter);

            warp.SetSeed(m_domain_warp_seed);
            warp.SetFrequency(m_domain_warp_frequency);
            warp.SetDomainWarpType(m_domain_warp_type);
            warp.SetDomainWarpAmp(m_domain_warp_amplitude);

            int bs = (m_domain_warp_fractal_type ? m_domain_warp_type + 3 : 0);
            //fnlWarp.SetFractalType((FastNoiseLite::FractalType)(fnlDomainWarpFractalType ? fnlDomainWarpFractalType + 3 : 0));
            warp.SetFractalType((FastNoiseLite::FractalType)bs);
            warp.SetFractalOctaves(m_domain_warp_fractal_octaves);
            warp.SetFractalLacunarity(m_domain_warp_fractal_lacunarity);
            warp.SetFractalGain(m_domain_warp_fractal_gain);

            return {noise, warp};
        }

        FastNoiseLite::NoiseType m_noise_type = FastNoiseLite::NoiseType::NoiseType_Perlin;
        int m_seed = 1337;
        float m_frequency = 0.01f;

        FastNoiseLite::FractalType m_fractal_type = FastNoiseLite::FractalType::FractalType_None;
        int m_fractal_octaves = 3;
        float m_fractal_lacunarity = 2.0f;
        float m_fractal_gain = 0.5f;
        float m_fractal_weighted_strength = 0.0f;
        float m_fractal_ping_pong_strength = 2.0f;

        FastNoiseLite::CellularDistanceFunction m_cellular_distance_function = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq;
        FastNoiseLite::CellularReturnType m_cellular_return_type;
        float m_cellular_jitter = 1.0f;

        int m_domain_warp_seed = 1337;
        float m_domain_warp_frequency = 0.01f;
        FastNoiseLite::DomainWarpType m_domain_warp_type = FastNoiseLite::DomainWarpType::DomainWarpType_BasicGrid;
        float m_domain_warp_amplitude = 1.0f;

        FastNoiseLite::FractalType m_domain_warp_fractal_type = FastNoiseLite::FractalType::FractalType_None;
        int m_domain_warp_fractal_octaves = 3;
        float m_domain_warp_fractal_lacunarity = 2.0f;
        float m_domain_warp_fractal_gain = 0.5f;
    };

    using noise_transform = std::function<float(float noise, glm::uvec2 pos)>;

    struct NoiseNode{
        std::vector<NoisePattern> noises;
        std::vector<noise_transform> noise_transforms;
    };

    template<typename T>
    class NoisePipeline{
        public:
            NoisePipeline(NoisePattern pattern):m_pattern(pattern){}
            T gen_noise(glm::vec2 pos){
                if (!init)
                    m_noise = m_pattern.get_noise();
                m_noise.warp.DomainWarp(pos.x,pos.y);
                float noise = m_noise.noise.GetNoise(pos.x, pos.y);
                return {noise, noise, noise};
            }
        private:
        NoisePattern m_pattern; 
        WarpedNoise m_noise;
        bool init = false;
    };

    template<typename T>
    class NoiseGenerator{
        public:
            NoiseGenerator(NoisePipeline<T> pipeline) : m_noise_pipeline(pipeline){
            }
            NoiseGenerator(std::string path_to_pipeline_json);
            NoiseGenerator(nlohmann::json pipepline_json);

            NoiseGenerator(NoiseGenerator&& ref) = default;//only for move

            NoiseGenerator() = delete;
            NoiseGenerator(const NoiseGenerator& ref) = delete;
            NoiseGenerator& operator=(const NoiseGenerator& ref) = delete;

            std::vector<T> generate_noise();
            void generate_noise(glm::uvec2 start, glm::uvec2 end, std::function<void(T, glm::uvec2)> f);
            T generate_noise(glm::vec2 pos){
                return m_noise_pipeline.gen_noise(pos);
            }
            //return the noise from all stages and combinations auto generate_noise();

            nlohmann::json get_serialized()const;
            void change_pipeline(NoisePipeline<T> pipeline);
            const NoisePipeline<T>& get_pipeline()const;
            NoisePipeline<T>& get_pipeline();
        private:
            NoisePipeline<T> m_noise_pipeline;
    };
}