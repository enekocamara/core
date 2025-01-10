#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "Syris/utils/containers/Singleton.hpp"
#include "Syris/utils/world_generation/ChunkSystem.hpp"
#include "Syris/utils/filesystem/FileWatcher.hpp"
#include "Syris/utils/filesystem/DLL_Watcher.hpp"

TEST(SampleTest, StaticTemplates) {
    Syris::Singleton<int>::init<int>(10);
    EXPECT_EQ(10, Syris::Singleton<int>::get());
}

TEST(SampleTest, Chunks) {
    Syris::ChunkSystem system({ 10,10 }, { 10,10 }, { 1.f,1.f });
    EXPECT_EQ(10, Syris::Singleton<int>::get());
}

/*
constexpr bool watch_file_test = true;
TEST(SampleTest, WatchFile) {
    if constexpr (watch_file_test) {
        Syris::FileWatcher watcher("C:\\Users\\eneko\\dev\\asharis\\game\\Build\\Debug\\Scripts.dll");
        while (true) {
            if (watcher.check_file() == Syris::FileWatcher::Event::Modified){
                std::cout << "file was changed\n";
                watcher.set_up_to_date();
            }
        }
    }
}*/

TEST(SampleTest, WatchDLL) {
    try{
        Syris::DLL_Watcher watcher("C:\\Users\\eneko\\dev\\asharis\\game\\Build\\dll\\Debug", "Scripts");
        while (true){
            watcher.check();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }catch(std::exception& e){
        FAIL() << e.what();
        return;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}