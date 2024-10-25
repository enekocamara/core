#pragma once

#include <unordered_map>
#include <optional>
#include <functional>

#include <entt.hpp>
#include "Syris/include/imgui.h"
#include "Syris/layers/Layer.hpp"
#include "Syris/utils/containers/BiMap.hpp"
#include "Types.hpp"
namespace Syris{
    //scheme to make categories
    //scene
    //  ->materials
    //      ->index
    //      ->vertex
    //we need a way to make a tree like structure

    /*
    class StatisticModule{
        public:
            struct CreateInfo{
                //std::string name;
               // StatisticModType type;
                std::optional<StatisticModID> parent;//if nonopt it means its the root
                entt::entity id;
            };
            StatisticModule(CreateInfo info){
                //m_name = info.name;
              //  m_type = info.type;
                m_parent = info.parent; 
            }
            void add_child(StatisticModID child){m_childs.push_back(child);}
            const std::vector<StatisticModID>& get_childs()const{return m_childs;}
            //std::string get_name() const {return m_name;}

        private:
            entt::entity m_id;
            //std::string m_name;
           // StatisticModType m_type;
            std::vector<StatisticModID> m_childs;
            std::optional<StatisticModID> m_parent;//if nonopt it means its the root
    };*/


    class Statistics : public Layer{
    public:
        Statistics();
        struct AddModuleInfo{
            std::optional<StatisticModID> parent;
            std::function<void(entt::entity, entt::registry&)> render;
            //void(*render)(entt::entity, entt::registry&);
        };
        StatisticModID add_module(AddModuleInfo info);
        entt::registry& get_registry(){return m_registry;}
        //StatisticModule& get_module(StatisticModID id){return m_modules[m_ids_x_index.from<StatisticModID>()[id]];};
        StatisticModID get_root(){return m_root_id;}
        void add_child(StatisticModID parent, StatisticModID child);
        void on_update(engine_time::Time &time) override;
        bool on_event(Event *event) override { return false; }
    private:
        StatisticModType add_mod_type(std::string mod_name)
        {
            auto &from_name = m_types_x_mod_names.from<SatisticModName>();
            auto it = from_name.find(mod_name);

            if (it != from_name.end())
                return it->second;

            StatisticModType type = m_current_mod_type++;
            m_types_x_mod_names.add(type, mod_name);
            return type;
        }
        template <class T>
        StatisticModType get_mod_type() {
            return add_mod_type(get_name<T>());
        }

        void render_module(StatisticModID id, bool recursive);
        entt::registry m_registry;
        StatisticModType m_current_mod_type = 1;
        BiMap<StatisticModType, SatisticModName> m_types_x_mod_names;
        StatisticModID m_root_id;
    };
}
