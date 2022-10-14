#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Float, float1) {
    std::string path = TEST_CASE_DIR "/float1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("flt1");
    ASSERT_EQ(n1.As<TOML::kFloat>()->Value(), 1.0);
    TOML::Node n2 = node.As<TOML::kTable>()->Get("flt2");
    ASSERT_EQ(n2.As<TOML::kFloat>()->Value(), 3.1415);
    TOML::Node n3 = node.As<TOML::kTable>()->Get("flt3");
    ASSERT_EQ(n3.As<TOML::kFloat>()->Value(), -0.01);
    TOML::Node n4 = node.As<TOML::kTable>()->Get("flt4");
    ASSERT_EQ(n4.As<TOML::kFloat>()->Value(), 5e+22);
    TOML::Node n5 = node.As<TOML::kTable>()->Get("flt5");
    ASSERT_EQ(n5.As<TOML::kFloat>()->Value(), 1e06);
    TOML::Node n6 = node.As<TOML::kTable>()->Get("flt6");
    ASSERT_EQ(n6.As<TOML::kFloat>()->Value(), -2E-2);
    TOML::Node n7 = node.As<TOML::kTable>()->Get("flt7");
    ASSERT_EQ(n7.As<TOML::kFloat>()->Value(), 6.626e-34);
}

TEST(Float, float2) {
    std::string path = TEST_CASE_DIR "/float2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, float3) {
    std::string path = TEST_CASE_DIR "/float3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, float4) {
    std::string path = TEST_CASE_DIR "/float4.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Float, float5) {
    std::string path = TEST_CASE_DIR "/float5.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("flt8");
    ASSERT_EQ(n1.As<TOML::kFloat>()->Value(), 224617.445991228);
}

TEST(Float, float6) {
    std::string path = TEST_CASE_DIR "/float6.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("sf1");
    ASSERT_TRUE(std::isinf(n1.As<TOML::kFloat>()->Value()));
    ASSERT_EQ(n1.As<TOML::kFloat>()->Value(), INFINITY);

    TOML::Node n2 = node.As<TOML::kTable>()->Get("sf2");
    ASSERT_TRUE(std::isinf(n2.As<TOML::kFloat>()->Value()));
    ASSERT_EQ(n2.As<TOML::kFloat>()->Value(), INFINITY);

    TOML::Node n3 = node.As<TOML::kTable>()->Get("sf3");
    ASSERT_TRUE(std::isinf(n3.As<TOML::kFloat>()->Value()));
    ASSERT_EQ(n3.As<TOML::kFloat>()->Value(), INFINITY * -1);

    TOML::Node n4 = node.As<TOML::kTable>()->Get("sf4");
    ASSERT_TRUE(std::isnan(n4.As<TOML::kFloat>()->Value()));

    TOML::Node n5 = node.As<TOML::kTable>()->Get("sf5");
    ASSERT_TRUE(std::isnan(n5.As<TOML::kFloat>()->Value()));

    TOML::Node n6 = node.As<TOML::kTable>()->Get("sf6");
    ASSERT_TRUE(std::isnan(n6.As<TOML::kFloat>()->Value()));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
