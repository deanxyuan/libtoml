#include <iostream>
#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(String, Comment) {
    std::string path = TEST_CASE_DIR "/comment.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
