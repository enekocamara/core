#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <expected>

inline std::expected<void, std::string> write_file(const std::string& file_name, const std::string& content) {
    // Open file in output mode, which will create or overwrite the file
    std::ofstream file(file_name);
    
    if (!file) 
        return std::unexpected(std::format("Error: Unable to open file {}.", file_name));

    // Write content to the file
    file << content;

    // Close the file automatically when going out of scope
    if (!file.good())
        return std::unexpected(std::format("Error: Writing to file {} failed.\n", file_name));
}