#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckObjectStringValue(TOML::Node node, const std::string &key, const std::string &value) {
    TOML::Node v = node.As<TOML::kObject>()->Get(key);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == value;
}

bool CheckObjectIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kObject>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

TEST(InlineTable, BasicTest) {
    std::string path = TEST_CASE_DIR "/inline_table.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node x = node.As<TOML::kObject>()->Get("x");
    ASSERT_EQ(x.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectIntValue(x, "a", 1));
    ASSERT_TRUE(CheckObjectIntValue(x, "b", 2));
}

TEST(InlineTable, SubTableTest) {
    std::string path = TEST_CASE_DIR "/inlinetab1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node name = node.As<TOML::kObject>()->Get("name");
    ASSERT_EQ(name.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(name, "first", "Tom"));
    ASSERT_TRUE(CheckObjectStringValue(name, "last", "Preston-Werner"));

    TOML::Node point = node.As<TOML::kObject>()->Get("point");
    ASSERT_EQ(point.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectIntValue(point, "x", 1));
    ASSERT_TRUE(CheckObjectIntValue(point, "y", 2));

    // animal = { type.name = "pug" }
    TOML::Node animal = node.As<TOML::kObject>()->Get("animal");
    ASSERT_EQ(animal.As<TOML::kObject>()->size(), 1);
    TOML::Node sub = animal.As<TOML::kObject>()->Get("type");
    ASSERT_EQ(sub.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectStringValue(sub, "name", "pug"));
}

TEST(InlineTable, InvalidTest1) {
    std::string path = TEST_CASE_DIR "/inlinetab2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(InlineTable, InvalidTest2) {
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
