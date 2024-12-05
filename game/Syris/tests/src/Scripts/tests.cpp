#include "Syris/scripts/DynamicScriptLoader.hpp"
#include "Syris/scripts/DynamicLibraryLoader.hpp"
#include <gtest/gtest.h>

TEST(SampleTest, DynamicScriptTest) {
    std::string dll_path = "C:\\Users\\eneko\\dev\\asharis\\game\\build\\Debug\\SyrisTestScripts.dll";
    try
    {
        Syris::DynamicScriptLoader<void()> nothing({dll_path, "nothing"});
        nothing();
        Syris::DynamicScriptLoader<void(int)> param_val({dll_path, "param_val"});
        param_val(1);
        Syris::DynamicScriptLoader<int()> return_val({dll_path, "return_val"});
        EXPECT_EQ(return_val(), 420);
        Syris::DynamicScriptLoader<int(int)> add_1({dll_path, "add_1"});
        EXPECT_EQ(add_1(1), 2);
        Syris::DynamicScriptLoader<void(int&)> ref_add_1({dll_path, "ref_add_1"});
        int i = 1;
        ref_add_1(i);
        EXPECT_EQ(i, 2);
    }catch(std::runtime_error& e){
        FAIL() << "Failed to create dynamic script loader: " << e.what();
    }
}

TEST(SampleTest, DynamicLibraryTest) {
    std::string dll_path = "C:\\Users\\eneko\\dev\\asharis\\game\\build\\Debug\\SyrisTestScripts.dll";
    try
    {
        Syris::DynamicLibraryLoader library({dll_path});

        auto namespaced = library.get_function_ptr<void(void)>("namespaced_chick");
        if (!namespaced)
            FAIL() << namespaced.error();
        namespaced.value()();

        auto param_val = library.get_function<void(int)>("param_val");
        if (!param_val)
            FAIL() << param_val.error();

        auto return_val = library.get_function<int()>("return_val");
        if (!return_val)
            FAIL() << return_val.error();
        EXPECT_EQ(return_val.value()(), 420);

        
        auto add_1 = library.get_function<int(int)>("add_1");
        if (!add_1)
            FAIL() << add_1.error();
        EXPECT_EQ(add_1.value()(1), 2);

        
        auto ref_add_1 = library.get_function<void(int&)>("ref_add_1");
        if (!ref_add_1)
            FAIL() << ref_add_1.error();
        int i = 1;
        ref_add_1.value()(i);
        EXPECT_EQ(i, 2);
    }catch(std::runtime_error& e){
        FAIL() << "Failed to create dynamic script loader: " << e.what();
    }
}