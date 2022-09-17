#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Boolean, Basic) {
    std::string path = TEST_CASE_DIR "/bool1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("bool1");
    ASSERT_TRUE(n1.As<TOML::kBoolean>()->Value());
    TOML::Node n2 = node.As<TOML::kObject>()->Get("bool2");
    ASSERT_FALSE(n2.As<TOML::kBoolean>()->Value());
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
