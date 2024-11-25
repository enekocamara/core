#pragma once
#include <unordered_map>
#include <string>
#include "Syris/types/Type.h"
namespace Syris{
    class Layout{
        public:
            Layout(){}
            ~Layout(){}
            bool add_variable(std::string& name, Type type);
            bool add_variable(std::string_view name, Type type);
            bool remove_variable(std::string& name);
            const std::unordered_map<std::string, Type>& get_vars() const{return m_variables;}
            const std::vector<std::string>& get_order()const {return m_order;}
            void render() const;
            void debug() const;
        private:
            std::unordered_map<std::string, Type> m_variables;
            std::vector<std::string> m_order;
    };
}