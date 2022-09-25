#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(KVPair, Basic) {
    std::string path = TEST_CASE_DIR "/kvpair0.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("key");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
}

TEST(KVPair, Inlvaid1) {
    std::string path = TEST_CASE_DIR "/kvpair1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(KVPair, Inlvaid2) {
    std::string path = TEST_CASE_DIR "/kvpair2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
