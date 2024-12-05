#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <expected>

namespace Syris{
    class DirWatcher{
        struct Item{
            enum class Type{
                File,
                Folder
            };
            enum class Status{
                UpToDate,
                Modified,
                Deleted
            };
            Type type;
            Status status;
        };

        enum class Status{
            FolderNotFound,
            Nominal
        };

        DirWatcher(std::string path);

        DirWatcher() = delete;
        DirWatcher(const DirWatcher &ref) = delete;
        DirWatcher &operator=(const DirWatcher &ref) = delete;

        std::vector<Item> check();
        private:
            std::string path;
            Status m_status;
            std::vector<Item> m_items;
    };
}