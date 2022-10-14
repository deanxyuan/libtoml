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

TEST(Keys, keys00) {
    std::string path = TEST_CASE_DIR "/keys00.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("key");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n2 = node.As<TOML::kTable>()->Get("bare_key");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n3 = node.As<TOML::kTable>()->Get("bare-key");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n4 = node.As<TOML::kTable>()->Get("1234");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), std::string("value"));
}

TEST(Keys, keys01) {
    std::string path = TEST_CASE_DIR "/keys01.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("127.0.0.1");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n2 = node.As<TOML::kTable>()->Get("character encoding");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n3 = node.As<TOML::kTable>()->Get("ʎǝʞ");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n4 = node.As<TOML::kTable>()->Get("key2");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n5 = node.As<TOML::kTable>()->Get("quoted \"value\"");
    ASSERT_EQ(n5.As<TOML::kString>()->Value(), std::string("value"));
}

TEST(Keys, keys02) {
    std::string path = TEST_CASE_DIR "/keys02.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Keys, keys03) {
    std::string path = TEST_CASE_DIR "/keys03.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("blank"));
}

TEST(Keys, keys04) {
    std::string path = TEST_CASE_DIR "/keys04.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("name");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("Orange"));

    // physical.color = "orange"
    // physical.shape = "round"
    TOML::Node physical = node.As<TOML::kTable>()->Get("physical");
    ASSERT_EQ(physical.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(physical, "color", "orange"));
    ASSERT_TRUE(CheckTableHasStringValue(physical, "shape", "round"));

    // site."google.com" = true
    TOML::Node site = node.As<TOML::kTable>()->Get("site");
    ASSERT_EQ(site.As<TOML::kTable>()->size(), 1);
    TOML::Node v = site.As<TOML::kTable>()->Get("google.com");
    ASSERT_EQ(v.Type(), TOML::kBoolean);
    ASSERT_TRUE(v.As<TOML::kBoolean>()->Value());
}

TEST(Keys, keys05) {
    std::string path = TEST_CASE_DIR "/keys05.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kTable>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(fruit, "color", "yellow"));
    ASSERT_TRUE(CheckTableHasStringValue(fruit, "name", "banana"));
    ASSERT_TRUE(CheckTableHasStringValue(fruit, "flavor", "banana"));
}

TEST(Keys, keys06) {
    std::string path = TEST_CASE_DIR "/keys06.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}
TEST(Keys, keys07) {
    std::string path = TEST_CASE_DIR "/keys07.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}
TEST(Keys, keys08) {
    std::string path = TEST_CASE_DIR "/keys08.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kTable>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasIntValue(fruit, "orange", 2));
    TOML::Node apple  = fruit.As<TOML::kTable>()->Get("apple");
    TOML::Node smooth = apple.As<TOML::kTable>()->Get("smooth");
    ASSERT_EQ(smooth.Type(), TOML::kBoolean);
    ASSERT_TRUE(smooth.As<TOML::kBoolean>()->Value());
}

TEST(Keys, keys09) {
    std::string path = TEST_CASE_DIR "/keys09.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Keys, keys10) {
    std::string path = TEST_CASE_DIR "/keys10.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node apple = node.As<TOML::kTable>()->Get("apple");
    ASSERT_EQ(apple.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(apple, "type", "fruit"));
    ASSERT_TRUE(CheckTableHasStringValue(apple, "skin", "thin"));
    ASSERT_TRUE(CheckTableHasStringValue(apple, "color", "red"));

    TOML::Node orange = node.As<TOML::kTable>()->Get("orange");
    ASSERT_EQ(orange.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(orange, "type", "fruit"));
    ASSERT_TRUE(CheckTableHasStringValue(orange, "skin", "thick"));
    ASSERT_TRUE(CheckTableHasStringValue(orange, "color", "orange"));
}

TEST(Keys, keys11) {
    std::string path = TEST_CASE_DIR "/keys11.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node apple = node.As<TOML::kTable>()->Get("apple");
    ASSERT_EQ(apple.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(apple, "type", "fruit"));
    ASSERT_TRUE(CheckTableHasStringValue(apple, "skin", "thin"));
    ASSERT_TRUE(CheckTableHasStringValue(apple, "color", "red"));

    TOML::Node orange = node.As<TOML::kTable>()->Get("orange");
    ASSERT_EQ(orange.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(orange, "type", "fruit"));
    ASSERT_TRUE(CheckTableHasStringValue(orange, "skin", "thick"));
    ASSERT_TRUE(CheckTableHasStringValue(orange, "color", "orange"));
}

TEST(Keys, keys12) {
    std::string path = TEST_CASE_DIR "/keys12.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kTable>()->Get("3");
    ASSERT_EQ(n1.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasStringValue(n1, "14159", "pi"));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
