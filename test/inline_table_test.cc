#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckTableHasStringValue(TOML::Node node, const std::string &key, const std::string &value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == value;
}

bool CheckTableHasIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

TEST(InlineTable, inline_table) {
    std::string path = TEST_CASE_DIR "/inline_table.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node x = node.As<TOML::kTable>()->Get("x");
    ASSERT_EQ(x.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasIntValue(x, "a", 1));
    ASSERT_TRUE(CheckTableHasIntValue(x, "b", 2));
}

TEST(InlineTable, inlinetab1) {
    std::string path = TEST_CASE_DIR "/inlinetab1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node name = node.As<TOML::kTable>()->Get("name");
    ASSERT_EQ(name.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(name, "first", "Tom"));
    ASSERT_TRUE(CheckTableHasStringValue(name, "last", "Preston-Werner"));

    TOML::Node point = node.As<TOML::kTable>()->Get("point");
    ASSERT_EQ(point.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasIntValue(point, "x", 1));
    ASSERT_TRUE(CheckTableHasIntValue(point, "y", 2));

    // animal = { type.name = "pug" }
    TOML::Node animal = node.As<TOML::kTable>()->Get("animal");
    ASSERT_EQ(animal.As<TOML::kTable>()->size(), 1);
    TOML::Node sub = animal.As<TOML::kTable>()->Get("type");
    ASSERT_EQ(sub.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasStringValue(sub, "name", "pug"));
}

TEST(InlineTable, inlinetab2) {
    std::string path = TEST_CASE_DIR "/inlinetab2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(InlineTable, inlinetab3) {
    std::string path = TEST_CASE_DIR "/inlinetab3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
