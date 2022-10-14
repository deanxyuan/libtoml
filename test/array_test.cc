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

TEST(Array, BasicTest) {
    std::string path = TEST_CASE_DIR "/arr1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("integers");
    ASSERT_EQ(n1.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayIntValue(n1, 0, 1));
    ASSERT_TRUE(CheckArrayIntValue(n1, 1, 2));
    ASSERT_TRUE(CheckArrayIntValue(n1, 2, 3));

    TOML::Node n2 = node.As<TOML::kTable>()->Get("colors");
    ASSERT_EQ(n2.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayStringValue(n2, 0, "red"));
    ASSERT_TRUE(CheckArrayStringValue(n2, 1, "yellow"));
    ASSERT_TRUE(CheckArrayStringValue(n2, 2, "green"));

    TOML::Node nested_arrays_of_ints = node.As<TOML::kTable>()->Get("nested_arrays_of_ints");
    ASSERT_EQ(nested_arrays_of_ints.As<TOML::kArray>()->size(), 2);
    TOML::Node n3n1 = nested_arrays_of_ints.As<TOML::kArray>()->At(0);
    ASSERT_EQ(n3n1.As<TOML::kArray>()->size(), 2);
    ASSERT_TRUE(CheckArrayIntValue(n3n1, 0, 1));
    ASSERT_TRUE(CheckArrayIntValue(n3n1, 1, 2));
    TOML::Node n3n2 = nested_arrays_of_ints.As<TOML::kArray>()->At(1);
    ASSERT_EQ(n3n2.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayIntValue(n3n2, 0, 3));
    ASSERT_TRUE(CheckArrayIntValue(n3n2, 1, 4));
    ASSERT_TRUE(CheckArrayIntValue(n3n2, 2, 5));

    TOML::Node nested_mixed_array = node.As<TOML::kTable>()->Get("nested_mixed_array");
    ASSERT_EQ(nested_mixed_array.As<TOML::kArray>()->size(), 2);
    TOML::Node n4n1 = nested_mixed_array.As<TOML::kArray>()->At(0);
    ASSERT_EQ(n4n1.As<TOML::kArray>()->size(), 2);
    ASSERT_TRUE(CheckArrayIntValue(n4n1, 0, 1));
    ASSERT_TRUE(CheckArrayIntValue(n4n1, 1, 2));
    TOML::Node n4n2 = nested_mixed_array.As<TOML::kArray>()->At(1);
    ASSERT_EQ(n4n2.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayStringValue(n4n2, 0, "a"));
    ASSERT_TRUE(CheckArrayStringValue(n4n2, 1, "b"));
    ASSERT_TRUE(CheckArrayStringValue(n4n2, 2, "c"));

    TOML::Node string_array = node.As<TOML::kTable>()->Get("string_array");
    ASSERT_EQ(string_array.As<TOML::kArray>()->size(), 4);
    ASSERT_TRUE(CheckArrayStringValue(string_array, 0, "all"));
    ASSERT_TRUE(CheckArrayStringValue(string_array, 1, "strings"));
    ASSERT_TRUE(CheckArrayStringValue(string_array, 2, "are the same"));
    ASSERT_TRUE(CheckArrayStringValue(string_array, 3, "type"));

    TOML::Node numbers = node.As<TOML::kTable>()->Get("numbers");
    ASSERT_EQ(numbers.As<TOML::kArray>()->size(), 6);
    TOML::Node f1 = numbers.As<TOML::kArray>()->At(0);
    ASSERT_FLOAT_EQ(f1.As<TOML::kFloat>()->Value(), 0.1);

    TOML::Node f2 = numbers.As<TOML::kArray>()->At(1);
    ASSERT_FLOAT_EQ(f2.As<TOML::kFloat>()->Value(), 0.2);

    TOML::Node f3 = numbers.As<TOML::kArray>()->At(2);
    ASSERT_FLOAT_EQ(f3.As<TOML::kFloat>()->Value(), 0.5);

    ASSERT_TRUE(CheckArrayIntValue(numbers, 3, 1));
    ASSERT_TRUE(CheckArrayIntValue(numbers, 4, 2));
    ASSERT_TRUE(CheckArrayIntValue(numbers, 5, 5));

    TOML::Node contributors = node.As<TOML::kTable>()->Get("contributors");
    ASSERT_EQ(contributors.As<TOML::kArray>()->size(), 2);
    ASSERT_TRUE(CheckArrayStringValue(contributors, 0, "Foo Bar <foo@example.com>"));
    TOML::Node n5n2 = contributors.As<TOML::kArray>()->At(1);
    ASSERT_EQ(n5n2.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(n5n2, "name", "Baz Qux"));
    ASSERT_TRUE(CheckObjectStringValue(n5n2, "email", "bazqux@example.com"));
    ASSERT_TRUE(CheckObjectStringValue(n5n2, "url", "https://example.com/bazqux"));
}

TEST(Array, MultiLineTest) {
    std::string path = TEST_CASE_DIR "/arr2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node integers2 = node.As<TOML::kTable>()->Get("integers2");
    ASSERT_EQ(integers2.As<TOML::kArray>()->size(), 3);
    ASSERT_TRUE(CheckArrayIntValue(integers2, 0, 1));
    ASSERT_TRUE(CheckArrayIntValue(integers2, 1, 2));
    ASSERT_TRUE(CheckArrayIntValue(integers2, 2, 3));

    TOML::Node integers3 = node.As<TOML::kTable>()->Get("integers3");
    ASSERT_EQ(integers3.As<TOML::kArray>()->size(), 2);
    ASSERT_TRUE(CheckArrayIntValue(integers3, 0, 1));
    ASSERT_TRUE(CheckArrayIntValue(integers3, 1, 2));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
