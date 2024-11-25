#pragma once
#include <string_view>
#include <vector>

namespace Syris{
    std::vector<std::string_view> view_split(const std::string& string, char delimiter){
        std::vector<std::string_view> views;
        size_t start = 0;
        size_t end;
        while ((end = string.find(delimiter, start)) != std::string::npos){
            views.emplace_back(string.data() + start, end - start);
            start = end + 1;
        }
        views.emplace_back(string.data() + start, string.size() - start);
        return views;
    }
    std::vector<std::string_view> view_split(std::string_view string, const char *delimiter){
        std::vector<std::string_view> views;
        size_t start = 0;
        size_t end;
        while ((end = string.find(delimiter, start)) != std::string::npos){
            views.emplace_back(string.data() + start, end - start);
            start = end + 1;
        }
        views.emplace_back(string.data() + start, string.size() - start);
        return views;
    }
}