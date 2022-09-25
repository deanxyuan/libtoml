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

TEST(Keys, BasicTest) {
    std::string path = TEST_CASE_DIR "/keys00.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("key");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("bare_key");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n3 = node.As<TOML::kObject>()->Get("bare-key");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n4 = node.As<TOML::kObject>()->Get("1234");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), std::string("value"));
}

TEST(Keys, StringKey) {
    std::string path = TEST_CASE_DIR "/keys01.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("127.0.0.1");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("character encoding");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n3 = node.As<TOML::kObject>()->Get("ʎǝʞ");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n4 = node.As<TOML::kObject>()->Get("key2");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), std::string("value"));
    TOML::Node n5 = node.As<TOML::kObject>()->Get("quoted \"value\"");
    ASSERT_EQ(n5.As<TOML::kString>()->Value(), std::string("value"));
}

TEST(Keys, InvalidTest1) {
    std::string path = TEST_CASE_DIR "/keys02.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Keys, EmptyKey) {
    std::string path = TEST_CASE_DIR "/keys03.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("blank"));
}

TEST(Keys, ComplexKey) {
    std::string path = TEST_CASE_DIR "/keys04.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("name");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("Orange"));

    // physical.color = "orange"
    // physical.shape = "round"
    TOML::Node physical = node.As<TOML::kObject>()->Get("physical");
    ASSERT_EQ(physical.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(physical, "color", "orange"));
    ASSERT_TRUE(CheckObjectStringValue(physical, "shape", "round"));

    // site."google.com" = true
    TOML::Node site = node.As<TOML::kObject>()->Get("site");
    ASSERT_EQ(site.As<TOML::kObject>()->size(), 1);
    TOML::Node v = site.As<TOML::kObject>()->Get("google.com");
    ASSERT_EQ(v.Type(), TOML::kBoolean);
    ASSERT_TRUE(v.As<TOML::kBoolean>()->Value());
}

TEST(Keys, ComplexKeyDefTable) {
    std::string path = TEST_CASE_DIR "/keys05.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kObject>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(fruit, "color", "yellow"));
    ASSERT_TRUE(CheckObjectStringValue(fruit, "name", "banana"));
    ASSERT_TRUE(CheckObjectStringValue(fruit, "flavor", "banana"));
}

TEST(Keys, RedefineTest1) {
    std::string path = TEST_CASE_DIR "/keys06.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Keys, TableMultipointAdd1) {
    std::string path = TEST_CASE_DIR "/keys08.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kObject>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectIntValue(fruit, "orange", 2));
    TOML::Node apple  = fruit.As<TOML::kObject>()->Get("apple");
    TOML::Node smooth = apple.As<TOML::kObject>()->Get("smooth");
    ASSERT_EQ(smooth.Type(), TOML::kBoolean);
    ASSERT_TRUE(smooth.As<TOML::kBoolean>()->Value());
}

TEST(Keys, RedefineTest2) {
    std::string path = TEST_CASE_DIR "/keys09.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Keys, TableMultipointAdd2) {
    std::string path = TEST_CASE_DIR "/keys10.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node apple = node.As<TOML::kObject>()->Get("apple");
    ASSERT_EQ(apple.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(apple, "type", "fruit"));
    ASSERT_TRUE(CheckObjectStringValue(apple, "skin", "thin"));
    ASSERT_TRUE(CheckObjectStringValue(apple, "color", "red"));

    TOML::Node orange = node.As<TOML::kObject>()->Get("orange");
    ASSERT_EQ(orange.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(orange, "type", "fruit"));
    ASSERT_TRUE(CheckObjectStringValue(orange, "skin", "thick"));
    ASSERT_TRUE(CheckObjectStringValue(orange, "color", "orange"));
}

TEST(Keys, TableMultipointAdd3) {
    std::string path = TEST_CASE_DIR "/keys11.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node apple = node.As<TOML::kObject>()->Get("apple");
    ASSERT_EQ(apple.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(apple, "type", "fruit"));
    ASSERT_TRUE(CheckObjectStringValue(apple, "skin", "thin"));
    ASSERT_TRUE(CheckObjectStringValue(apple, "color", "red"));

    TOML::Node orange = node.As<TOML::kObject>()->Get("orange");
    ASSERT_EQ(orange.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(orange, "type", "fruit"));
    ASSERT_TRUE(CheckObjectStringValue(orange, "skin", "thick"));
    ASSERT_TRUE(CheckObjectStringValue(orange, "color", "orange"));
}

TEST(Keys, RawFloatTreatedAsTable) {
    std::string path = TEST_CASE_DIR "/keys12.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kObject>()->Get("3");
    ASSERT_EQ(n1.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectStringValue(n1, "14159", "pi"));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
