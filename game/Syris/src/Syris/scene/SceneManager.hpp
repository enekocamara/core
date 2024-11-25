#pragma once
#include <unordered_map>

#include "Scene.hpp"
#include "Syris/statistics/Statistics.hpp"

namespace Syris{
    using SceneID = uint64_t;
    class SceneManager{
        public:
            struct CreateInfo{
                Statistics& statistics;
            };
            SceneManager(CreateInfo info);
            
            SceneManager(const SceneManager& ref) = delete;
            ~SceneManager();
            /** @param scene mem owning pointer to a scene */
            SceneID new_scene(std::unique_ptr<Scene> scene);
            Scene* get_scene(SceneID id){return m_scenes[id].get();}
            void add_scene_to_statistics(StatisticModID id);
            void render_statistics(StatisticModID id, entt::registry& registry);
        private:
            std::unordered_map<SceneID, std::unique_ptr<Scene>> m_scenes;
            Statistics &m_statistics;
            SceneID m_current_scene_ID = 1;
            StatisticModID m_statistics_mod_ID;
    };
}