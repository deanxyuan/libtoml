#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckTableHasIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

bool CheckArrayHasIntValue(TOML::Node node, int index, int64_t s) {
    TOML::Node v = node.As<TOML::kArray>()->At(index);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == s;
}

TEST(InlineTable, array_of_tables) {
    std::string path = TEST_CASE_DIR "/array_of_tables.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node x = node.As<TOML::kTable>()->Get("x");
    ASSERT_EQ(x.Type(), TOML::kArray);
    ASSERT_EQ(x.As<TOML::kArray>()->size(), 2);
    TOML::Node N1 = x.As<TOML::kArray>()->At(0);
    ASSERT_EQ(N1.Type(), TOML::kTable);
    ASSERT_TRUE(CheckTableHasIntValue(N1, "a", 1));

    TOML::Node N2 = x.As<TOML::kArray>()->At(1);
    ASSERT_EQ(N2.Type(), TOML::kTable);
    ASSERT_TRUE(CheckTableHasIntValue(N2, "a", 2));
}

TEST(InlineTable, inline_array) {
    std::string path = TEST_CASE_DIR "/inline_array.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node x = node.As<TOML::kTable>()->Get("x");
    ASSERT_EQ(x.Type(), TOML::kArray);
    ASSERT_EQ(x.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayHasIntValue(x, 0, 1));
    ASSERT_TRUE(CheckArrayHasIntValue(x, 1, 2));
    ASSERT_TRUE(CheckArrayHasIntValue(x, 2, 3));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
