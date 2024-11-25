#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
namespace utils{
    inline std::string read_file(const char * filename)
    {
        std::string source;
        std::ifstream fileStream(filename, std::ios::in);
        if (fileStream.is_open()) {
            std::stringstream sstr;
            sstr << fileStream.rdbuf();
            source = sstr.str();
            fileStream.close();
            return source;
        }
        else {
            throw std::runtime_error(std::format("Failed to open file {}", filename));
        }
    }
}
