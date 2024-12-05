#include "Syris/utils/containers/VectorStorage.hpp"
#include <gtest/gtest.h>

TEST(SampleTest, VectorStorage) {
    Syris::VectorStorage<int> storage;
    storage.push<int>(10);
    storage.push<int>(1);
    storage.push<int>(2);


    EXPECT_EQ(10, storage.get<int>(0));
    EXPECT_EQ(1, storage.get<int>(1));
    EXPECT_EQ(2, storage.get<int>(2));
}