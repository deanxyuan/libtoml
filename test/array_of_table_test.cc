#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckArrayStringValue(TOML::Node node, int index, const std::string &s) {
    TOML::Node v = node.As<TOML::kArray>()->At(index);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == s;
}

bool CheckArrayFloatValue(TOML::Node node, int index, float s) {
    TOML::Node v = node.As<TOML::kArray>()->At(index);
    if (v.Type() != TOML::kFloat) {
        return false;
    }
    return v.As<TOML::kFloat>()->Value() == s;
}

bool CheckArrayIntValue(TOML::Node node, int index, int64_t s) {
    TOML::Node v = node.As<TOML::kArray>()->At(index);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == s;
}

bool CheckObjectStringValue(TOML::Node node, const std::string &key, const std::string &value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == value;
}

bool CheckObjectIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

TEST(Array, arrtab1) {
    std::string path = TEST_CASE_DIR "/arrtab1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node products = node.AsTable()->Get("products");
    ASSERT_EQ(products.Type(), TOML::kArray);
    ASSERT_EQ(products.As<TOML::kArray>()->size(), 3);

    TOML::Node n1 = products.AsArray()->At(0);
    ASSERT_EQ(n1.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(n1, "name", "Hammer"));
    ASSERT_TRUE(CheckObjectIntValue(n1, "sku", 738594937));

    TOML::Node n2 = products.AsArray()->At(1);
    ASSERT_EQ(n2.Type(), TOML::kTable);
    ASSERT_EQ(n2.AsTable()->size(), 0);

    TOML::Node n3 = products.AsArray()->At(2);
    ASSERT_EQ(n3.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(n3, "name", "Nail"));
    ASSERT_TRUE(CheckObjectIntValue(n3, "sku", 284758393));
    ASSERT_TRUE(CheckObjectStringValue(n3, "color", "gray"));
}

TEST(Array, arrtab2) {
    std::string path = TEST_CASE_DIR "/arrtab2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node fruits = node.AsTable()->Get("fruits");
    ASSERT_EQ(fruits.Type(), TOML::kArray);
    ASSERT_EQ(fruits.As<TOML::kArray>()->size(), 2);

    TOML::Node n1 = fruits.AsArray()->At(0);
    ASSERT_EQ(n1.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(n1, "name", "apple"));
    TOML::Node physical = n1.AsTable()->Get("physical");
    ASSERT_TRUE(CheckObjectStringValue(physical, "color", "red"));
    ASSERT_TRUE(CheckObjectStringValue(physical, "shape", "round"));
    TOML::Node varieties = n1.AsTable()->Get("varieties");
    ASSERT_EQ(varieties.Type(), TOML::kArray);
    ASSERT_EQ(varieties.As<TOML::kArray>()->size(), 2);
    TOML::Node e1 = varieties.As<TOML::kArray>()->At(0);
    TOML::Node e2 = varieties.As<TOML::kArray>()->At(1);
    ASSERT_TRUE(CheckObjectStringValue(e1, "name", "red delicious"));
    ASSERT_TRUE(CheckObjectStringValue(e2, "name", "granny smith"));

    TOML::Node n2 = fruits.AsArray()->At(1);
    ASSERT_EQ(n2.Type(), TOML::kTable);
    ASSERT_EQ(n2.AsTable()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(n2, "name", "banana"));

    varieties = n2.AsTable()->Get("varieties");
    ASSERT_EQ(varieties.Type(), TOML::kArray);
    ASSERT_EQ(varieties.As<TOML::kArray>()->size(), 1);
    TOML::Node e3 = varieties.As<TOML::kArray>()->At(0);
    ASSERT_TRUE(CheckObjectStringValue(e3, "name", "plantain"));
}

TEST(Array, arrtab3) {
    std::string path = TEST_CASE_DIR "/arrtab3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Array, arrtab4) {
    std::string path = TEST_CASE_DIR "/arrtab4.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Array, arrtab5) {
    std::string path = TEST_CASE_DIR "/arrtab5.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Array, arrtab6) {
    std::string path = TEST_CASE_DIR "/arrtab6.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Array, arrtab7) {
    std::string path = TEST_CASE_DIR "/arrtab7.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node points = node.As<TOML::kTable>()->Get("points");
    ASSERT_EQ(points.As<TOML::kArray>()->size(), 3);

    TOML::Node n1 = points.AsArray()->At(0);
    ASSERT_EQ(n1.Type(), TOML::kTable);
    ASSERT_EQ(n1.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectIntValue(n1, "x", 1));
    ASSERT_TRUE(CheckObjectIntValue(n1, "y", 2));
    ASSERT_TRUE(CheckObjectIntValue(n1, "z", 3));

    TOML::Node n2 = points.AsArray()->At(1);
    ASSERT_EQ(n2.Type(), TOML::kTable);
    ASSERT_EQ(n2.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectIntValue(n2, "x", 7));
    ASSERT_TRUE(CheckObjectIntValue(n2, "y", 8));
    ASSERT_TRUE(CheckObjectIntValue(n2, "z", 9));

    TOML::Node n3 = points.AsArray()->At(2);
    ASSERT_EQ(n3.Type(), TOML::kTable);
    ASSERT_EQ(n3.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectIntValue(n3, "x", 2));
    ASSERT_TRUE(CheckObjectIntValue(n3, "y", 4));
    ASSERT_TRUE(CheckObjectIntValue(n3, "z", 8));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
