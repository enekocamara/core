#include "TextureEditor.hpp"
#include "Syris/include/imgui.h"

namespace Syris{

    void TextureEditor::update_texture(bool new_preview)
    {
        if (!new_preview)
            return;
        for (int y = 0; y < m_frame_texture.get_size().y; y++)
        {
            for (int x = 0; x < m_frame_texture.get_size().x; x++)
            {
                double posX = x;
                double posY = y;
                if (m_pattern.m_domain_warp_type > 0)
                    m_warp.DomainWarp(posX, posY);
                float noise = m_noise.GetNoise(posX, posY);
                m_data.push_back({noise, noise, noise});
            }
        }
        m_frame_texture.get_texture()->update(m_data);
        m_data.clear();
        m_frame_texture.render();
        /*
        previewGenTime += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - timer).count() / 1000000.f;

        if (previewPixelY >= noiseTexSizeGenY)
        {
            noiseTexSize.x = noiseTexSizeGenX;
            noiseTexSize.y = noiseTexSizeGenY;
            previewPixelY = 0;
            previewMeanFinal = previewMean / (noiseTexSize.x * noiseTexSize.y);
            previewMinFinal = previewMin;
            previewMaxFinal = previewMax;
            previewGenTimeFinal = previewGenTime;

            if (noiseTex != 0)
            {
                glDeleteTextures(1, &noiseTex);
            }

            // Create a OpenGL texture identifier
            glGenTextures(1, &noiseTex);
            glBindTexture(GL_TEXTURE_2D, noiseTex);

            // Setup filtering parameters for display
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

            // Upload pixels into texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, noiseTexSizeGenX, noiseTexSizeGenY, 0, GL_RGBA, GL_UNSIGNED_BYTE, previewPixelArray);
        }
        */
    }


    bool TextureEditor::render(){
        static const char *enumNoiseType[] = {"OpenSimplex2", "OpenSimplex2S", "Cellular", "Perlin", "Value Cubic", "Value"};
        static const char *enumRotationType[] = {"None", "Improve XY Planes", "Improve XZ Planes"};
        static const char *enumFractalType[] = {"None", "FBm", "Ridged", "Ping Pong"};
        static const char *enumCellularType[] = {"Euclidean", "Euclidean Sq", "Manhattan", "Hybrid"};
        static const char *enumCellularReturnType[] = {"Cell Value", "Distance", "Distance 2", "Distance 2 Add", "Distance 2 Sub", "Distance 2 Mul", "Distance 2 Div"};
        static const char *enumDomainWarpType[] = {"None", "OpenSimplex2", "OpenSimplex2 Reduced", "Basic Grid"};
        static const char *enumDomainWarpFractalType[] = {"None", "Progressive", "Independent"};

        bool texUpdate = false;
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
        //ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
        //windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        //windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin(m_name.c_str(), &dockSpaceOpen);
        {
            // Create the dock space
            ImGuiID dockSpaceID = ImGui::GetID(m_name.c_str());
            ImGui::DockSpace(dockSpaceID);
            //ImGui::BeginTabBar("Tabs");
            ImGui::Begin("Settings");
            {
                /// General
                ImGui::TextUnformatted("General");

                if (ImGui::Combo("Noise Type", (int *)&m_pattern.m_noise_type, enumNoiseType, IM_ARRAYSIZE(enumNoiseType)))
                {
                    m_noise.SetNoiseType(m_pattern.m_noise_type);
                    texUpdate = true;
                }
                if (ImGui::DragInt("Seed", &m_pattern.m_seed))
                {
                    m_noise.SetSeed(m_pattern.m_seed);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Frequency", &m_pattern.m_frequency, 0.0002f))
                {
                    m_noise.SetFrequency(m_pattern.m_frequency);
                    texUpdate = true;
                }

                /// Fractal
                ImGui::TextUnformatted("Fractal");

                if (ImGui::Combo("Type", (int *)&m_pattern.m_fractal_type, enumFractalType, IM_ARRAYSIZE(enumFractalType)))
                {
                    m_noise.SetFractalType(m_pattern.m_fractal_type);
                    texUpdate = true;
                }
                ImGui::BeginDisabled(m_pattern.m_fractal_type == 0);
                if (ImGui::DragInt("Octaves", &m_pattern.m_fractal_octaves, 0.1f, 1, 20))
                {
                    m_noise.SetFractalOctaves(m_pattern.m_fractal_octaves);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Lacunarity", &m_pattern.m_fractal_lacunarity, 0.01f))
                {
                    m_noise.SetFractalLacunarity(m_pattern.m_fractal_lacunarity);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Gain", &m_pattern.m_fractal_gain, 0.01f))
                {
                    m_noise.SetFractalGain(m_pattern.m_fractal_gain);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Weighted Strength", &m_pattern.m_fractal_weighted_strength, 0.01f))
                {
                    m_noise.SetFractalWeightedStrength(m_pattern.m_fractal_weighted_strength);
                    texUpdate = true;
                }
                ImGui::BeginDisabled(m_pattern.m_fractal_type != FastNoiseLite::FractalType_PingPong);
                if (ImGui::DragFloat("Ping Pong Strength", &m_pattern.m_fractal_ping_pong_strength, 0.01f))
                {
                    m_noise.SetFractalPingPongStrength(m_pattern.m_fractal_ping_pong_strength);
                    texUpdate = true;
                }
                ImGui::EndDisabled();
                ImGui::EndDisabled();

                /// Cellular
                ImGui::TextUnformatted("Cellular");

                ImGui::BeginDisabled(m_pattern.m_noise_type != FastNoiseLite::NoiseType_Cellular);
                if (ImGui::Combo("Distance Function", (int *)&m_pattern.m_cellular_distance_function, enumCellularType, IM_ARRAYSIZE(enumCellularType)))
                {
                    m_noise.SetCellularDistanceFunction(m_pattern.m_cellular_distance_function);
                    texUpdate = true;
                }
                if (ImGui::Combo("Return Type", (int *)&m_pattern.m_cellular_return_type, enumCellularReturnType, IM_ARRAYSIZE(enumCellularReturnType)))
                {
                    m_noise.SetCellularReturnType(m_pattern.m_cellular_return_type);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Jitter", &m_pattern.m_cellular_jitter, 0.01f))
                {
                    m_noise.SetCellularJitter(m_pattern.m_cellular_jitter);
                    texUpdate = true;
                }
                ImGui::EndDisabled();

                /// Domain Warp
                ImGui::PushID("Domain Warp");
                ImGui::TextUnformatted("Domain Warp");

                if (ImGui::Combo("Type", (int *)&m_pattern.m_domain_warp_type, enumDomainWarpType, IM_ARRAYSIZE(enumDomainWarpType)))
                {
                    m_warp.SetDomainWarpType((FastNoiseLite::DomainWarpType)(m_pattern.m_domain_warp_type - 1));
                    texUpdate = true;
                }
                ImGui::BeginDisabled(m_pattern.m_domain_warp_type == 0);
                if (ImGui::DragFloat("Amplitude", &m_pattern.m_domain_warp_amplitude, 0.5f))
                {
                    m_warp.SetDomainWarpAmp(m_pattern.m_domain_warp_amplitude);
                    texUpdate = true;
                }
                if (ImGui::DragInt("Seed", &m_pattern.m_domain_warp_seed))
                {
                    m_warp.SetSeed(m_pattern.m_domain_warp_seed);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Frequency", &m_pattern.m_domain_warp_frequency, 0.001f))
                {
                    m_warp.SetFrequency(m_pattern.m_domain_warp_frequency);
                    texUpdate = true;
                }

                /// Domain Warp Fractal
                ImGui::PushID("Domain Warp Fractal");
                ImGui::TextUnformatted("Domain Warp Fractal");

                if (ImGui::Combo("Type", (int *)&m_pattern.m_domain_warp_fractal_type, enumDomainWarpFractalType, IM_ARRAYSIZE(enumDomainWarpFractalType)))
                {
                    m_warp.SetFractalType((FastNoiseLite::FractalType)(m_pattern.m_domain_warp_fractal_type ? m_pattern.m_domain_warp_fractal_type + 3 : 0));
                    texUpdate = true;
                }
                ImGui::BeginDisabled(m_pattern.m_domain_warp_fractal_type == 0);
                if (ImGui::DragInt("Octaves", &m_pattern.m_domain_warp_fractal_octaves, 0.1f, 1, 20))
                {
                    m_warp.SetFractalOctaves(m_pattern.m_domain_warp_fractal_octaves);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Lacunarity", &m_pattern.m_domain_warp_fractal_lacunarity, 0.01f))
                {
                    m_warp.SetFractalLacunarity(m_pattern.m_domain_warp_fractal_lacunarity);
                    texUpdate = true;
                }
                if (ImGui::DragFloat("Gain", &m_pattern.m_domain_warp_fractal_gain, 0.01f))
                {
                    m_warp.SetFractalGain(m_pattern.m_domain_warp_fractal_gain);
                    texUpdate = true;
                }
                ImGui::EndDisabled();
                ImGui::EndDisabled();
                ImGui::PopID();
                ImGui::PopID();
                ImGui::Begin("Noise Texture");
                {
                    update_texture(texUpdate);

                    ImGui::Image((void*)(intptr_t)m_frame_texture.get_texture_id(), { 400, 400 });
                }
                ImGui::End();
//                ImGui::EndTabItem();
            }
            ImGui::End();
            //ImGui::PopItemWidth();

           
        }
        ImGui::End();
        return texUpdate;      
    }
}