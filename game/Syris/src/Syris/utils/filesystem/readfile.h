#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <expected>

namespace utils{
    inline std::expected<std::string, std::string> read_file(const char * filename)
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
            return std::unexpected(std::format("Failed to open file {}", filename));
        }
    }
}
