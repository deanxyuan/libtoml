#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Integer, DecimalInteger) {
    std::string path = TEST_CASE_DIR "/int0.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kObject>()->Get("int1");
    ASSERT_EQ(n1.As<TOML::kInteger>()->Value(), 99);

    TOML::Node n2 = node.As<TOML::kObject>()->Get("int2");
    ASSERT_EQ(n2.As<TOML::kInteger>()->Value(), 42);

    TOML::Node n3 = node.As<TOML::kObject>()->Get("int3");
    ASSERT_EQ(n3.As<TOML::kInteger>()->Value(), 0);

    TOML::Node n4 = node.As<TOML::kObject>()->Get("int4");
    ASSERT_EQ(n4.As<TOML::kInteger>()->Value(), -17);

    TOML::Node n5 = node.As<TOML::kObject>()->Get("int5");
    ASSERT_EQ(n5.As<TOML::kInteger>()->Value(), 1000);

    TOML::Node n6 = node.As<TOML::kObject>()->Get("int6");
    ASSERT_EQ(n6.As<TOML::kInteger>()->Value(), 5349221);

    TOML::Node n7 = node.As<TOML::kObject>()->Get("int7");
    ASSERT_EQ(n7.As<TOML::kInteger>()->Value(), 5349221);

    TOML::Node n8 = node.As<TOML::kObject>()->Get("int8");
    ASSERT_EQ(n8.As<TOML::kInteger>()->Value(), 12345);
}

TEST(Integer, OtherDecimalInteger) {
    std::string path = TEST_CASE_DIR "/int1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kObject>()->Get("hex1");
    ASSERT_EQ(n1.As<TOML::kInteger>()->Value<false>(), 0xDEADBEEF);

    TOML::Node n2 = node.As<TOML::kObject>()->Get("hex2");
    ASSERT_EQ(n2.As<TOML::kInteger>()->Value<false>(), 0xdeadbeef);

    TOML::Node n3 = node.As<TOML::kObject>()->Get("hex3");
    ASSERT_EQ(n3.As<TOML::kInteger>()->Value<false>(), 0xdeadbeef);

    TOML::Node n4 = node.As<TOML::kObject>()->Get("oct1");
    ASSERT_EQ(n4.As<TOML::kInteger>()->Value(), 01234567);

    TOML::Node n5 = node.As<TOML::kObject>()->Get("oct2");
    ASSERT_EQ(n5.As<TOML::kInteger>()->Value(), 0755);

    TOML::Node n6 = node.As<TOML::kObject>()->Get("bin1");
    ASSERT_EQ(n6.As<TOML::kInteger>()->Value(), 0b11010110);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
