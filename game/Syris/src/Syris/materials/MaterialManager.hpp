#pragma once
#include <unordered_map>
#include "Syris/statistics/Statistics.hpp"
//#include "Material.hpp"
#include "CreateMaterial.hpp"
#include <vector>

namespace Syris{
    class MaterialManager{
        public:
            struct CreateInfo{
                Statistics& statistics;
            };
            using MaterialID = uint32_t;

            MaterialManager(CreateInfo info);
            ~MaterialManager();
            MaterialManager(const MaterialManager& ref) = delete;

            template<typename T>
            MaterialID add_material(Material::CreateInfo& info) {
                Material* material = create_material<T>(info);
                m_statistics.add_child(m_statistic_mod, material->get_statistics());
                MaterialID id = m_current_id++;
                m_materials[id] = material;
                return id;
            }
            

            void set_entity(MaterialID id, MaterialSetRequest& request);
            void draw(MaterialID id, void *uniforms);
            Material* get_material(MaterialID id);
            StatisticModID get_statistics(){return m_statistic_mod;}
            void render(entt::entity entity, entt::registry& registry);
        private:
            std::unordered_map<MaterialID, Material *> m_materials;
            MaterialID m_current_id = 1;
            Statistics& m_statistics;
            StatisticModID m_statistic_mod;
    };
}
