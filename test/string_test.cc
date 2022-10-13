#include <iostream>
#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(String, string0) {
    std::string path = TEST_CASE_DIR "/string0.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("str");
    // "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."
    std::string value = "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF.";
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), value);

    unsigned char buff[] = {0xC3, 0xA9, 0x00, 0x00};
    std::string s1       = "I'm a string. \"You can quote me\". Name\tJos";
    s1.append((char *)buff, 2);
    s1.append("\nLocation\tSF.");
    EXPECT_EQ(value, s1);
}

TEST(String, string1) {
    std::string path = TEST_CASE_DIR "/string1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    std::string value("Roses are red\nViolets are blue");
    TOML::Node n1 = node.As<TOML::kObject>()->Get("str1");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), value);

#ifdef _WIN32
    value         = std::string("Roses are red\r\nViolets are blue");
    TOML::Node n2 = node.As<TOML::kObject>()->Get("str3");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), value);
#else
    value         = std::string("Roses are red\nViolets are blue");
    TOML::Node n2 = node.As<TOML::kObject>()->Get("str2");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), value);
#endif
}

TEST(String, string3) {
    std::string path = TEST_CASE_DIR "/string3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    std::string value("The quick brown fox jumps over the lazy dog.");
    TOML::Node n1 = node.As<TOML::kObject>()->Get("str1");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), value);
    TOML::Node n2 = node.As<TOML::kObject>()->Get("str2");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), value);
    TOML::Node n3 = node.As<TOML::kObject>()->Get("str3");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), value);
}

TEST(String, string4) {
    std::string path = TEST_CASE_DIR "/string4.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    std::string str4("Here are two quotation marks: \"\". Simple enough.");
    TOML::Node n1 = node.As<TOML::kObject>()->Get("str4");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), str4);

    std::string str5("Here are three quotation marks: \"\"\".");
    TOML::Node n2 = node.As<TOML::kObject>()->Get("str5");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), str5);

    std::string str6("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\".");
    TOML::Node n3 = node.As<TOML::kObject>()->Get("str6");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), str6);

    std::string str7("\"This,\" she said, \"is just a pointless statement.\"");
    TOML::Node n4 = node.As<TOML::kObject>()->Get("str7");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), str7);
}

TEST(String, string5) {
    std::string path = TEST_CASE_DIR "/string5.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("winpath");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string(R"(C:\Users\nodejs\templates)"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("winpath2");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string(R"(\\ServerX\admin$\system32\)"));
    TOML::Node n3 = node.As<TOML::kObject>()->Get("quoted");
    ASSERT_EQ(n3.As<TOML::kString>()->Value(), std::string(R"(Tom "Dubs" Preston-Werner)"));
    TOML::Node n4 = node.As<TOML::kObject>()->Get("regex");
    ASSERT_EQ(n4.As<TOML::kString>()->Value(), std::string(R"(<\i\c*\s*>)"));
}

TEST(String, string6) {
    std::string path = TEST_CASE_DIR "/string6.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("regex2");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string(R"(I [dw]on't need \d{2} apples)"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("lines");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(),
              std::string("The first newline is\ntrimmed in raw strings.\n   All other "
                          "whitespace\n   is preserved.\n"));
}

TEST(String, string7) {
    std::string path = TEST_CASE_DIR "/string7.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("quot15");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(),
              std::string(R"(Here are fifteen quotation marks: """"""""""""""")"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("str");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(),
              std::string(R"('That,' she said, 'is still pointless.')"));
}

TEST(String, string8) {
    std::string path = TEST_CASE_DIR "/string8.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kObject>()->Get("apos15");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(),
              std::string(R"(Here are fifteen apostrophes: ''''''''''''''')"));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
