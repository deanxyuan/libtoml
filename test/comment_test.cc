#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Comment, CommentWithString) {
    std::string path = TEST_CASE_DIR "/comment.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("key");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n2 = node.As<TOML::kTable>()->Get("another");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("# This is not a comment"));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
