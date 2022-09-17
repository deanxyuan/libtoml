#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Float, Basic) {
    std::string path = TEST_CASE_DIR "/float1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("flt1");
    ASSERT_EQ(n1.As<TOML::kDouble>()->Value(), 1.0);
    TOML::Node n2 = node.As<TOML::kObject>()->Get("flt2");
    ASSERT_EQ(n2.As<TOML::kDouble>()->Value(), 3.1415);
    TOML::Node n3 = node.As<TOML::kObject>()->Get("flt3");
    ASSERT_EQ(n3.As<TOML::kDouble>()->Value(), -0.01);
    TOML::Node n4 = node.As<TOML::kObject>()->Get("flt4");
    ASSERT_EQ(n4.As<TOML::kDouble>()->Value(), 5e+22);
    TOML::Node n5 = node.As<TOML::kObject>()->Get("flt5");
    ASSERT_EQ(n5.As<TOML::kDouble>()->Value(), 1e06);
    TOML::Node n6 = node.As<TOML::kObject>()->Get("flt6");
    ASSERT_EQ(n6.As<TOML::kDouble>()->Value(), -2E-2);
    TOML::Node n7 = node.As<TOML::kObject>()->Get("flt7");
    ASSERT_EQ(n7.As<TOML::kDouble>()->Value(), 6.626e-34);
}

TEST(Float, InvalidDotMissPrefix) {
    std::string path = TEST_CASE_DIR "/float2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, InvalidDotMissSuffix) {
    std::string path = TEST_CASE_DIR "/float3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, InvalidDotAndE) {
    std::string path = TEST_CASE_DIR "/float4.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, SplicingChar) {
    std::string path = TEST_CASE_DIR "/float5.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("flt8");
    ASSERT_EQ(n1.As<TOML::kDouble>()->Value(), 224617.445991228);
}

TEST(Float, InfAndNan) {
    std::string path = TEST_CASE_DIR "/float6.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("sf1");
    ASSERT_TRUE(std::isinf(n1.As<TOML::kDouble>()->Value()));
    ASSERT_EQ(n1.As<TOML::kDouble>()->Value(), INFINITY);

    TOML::Node n2 = node.As<TOML::kObject>()->Get("sf2");
    ASSERT_TRUE(std::isinf(n2.As<TOML::kDouble>()->Value()));
    ASSERT_EQ(n2.As<TOML::kDouble>()->Value(), INFINITY);

    TOML::Node n3 = node.As<TOML::kObject>()->Get("sf3");
    ASSERT_TRUE(std::isinf(n3.As<TOML::kDouble>()->Value()));
    ASSERT_EQ(n3.As<TOML::kDouble>()->Value(), INFINITY * -1);

    TOML::Node n4 = node.As<TOML::kObject>()->Get("sf4");
    ASSERT_TRUE(std::isnan(n4.As<TOML::kDouble>()->Value()));

    TOML::Node n5 = node.As<TOML::kObject>()->Get("sf5");
    ASSERT_TRUE(std::isnan(n5.As<TOML::kDouble>()->Value()));

    TOML::Node n6 = node.As<TOML::kObject>()->Get("sf6");
    ASSERT_TRUE(std::isnan(n6.As<TOML::kDouble>()->Value()));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
