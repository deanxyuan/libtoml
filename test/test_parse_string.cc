/*
 *
 * Copyright 2022-2023 libtoml authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "toml/toml.h"
#include "util/testutil.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(String, string0) {
    std::string path = TEST_CASE_DIR "/string0.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("str");
    // "I'm a string. \"You can quote me\". Name\tJosé\nLocation\tSF."
    std::string value = "I'm a string. \"You can quote me\". Name\tJosé\nLocation\tSF.";
    ASSERT_EQ(n1.as_string(), value);

    unsigned char buff[] = {0xC3, 0xA9, 0x00, 0x00};
    std::string s1       = "I'm a string. \"You can quote me\". Name\tJos";
    s1.append((char *)buff, 2);
    s1.append("\nLocation\tSF.");
    ASSERT_EQ(value, s1);
}

TEST(String, string1) {
    std::string path = TEST_CASE_DIR "/string1.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    std::string value("Roses are red\nViolets are blue");
    const auto& n1 = node.as_table().at("str1");
    ASSERT_EQ(n1.as_string(), value);

#ifdef _WIN32
    value         = std::string("Roses are red\r\nViolets are blue");
    const auto& n2 = node.as_table().at("str3");
    ASSERT_EQ(n2.as_string(), value);
#else
    value         = std::string("Roses are red\nViolets are blue");
    const auto& n2 = node.as_table().at("str2");
    ASSERT_EQ(n2.as_string(), value);
#endif
}

TEST(String, string3) {
    std::string path = TEST_CASE_DIR "/string3.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    std::string value("The quick brown fox jumps over the lazy dog.");
    const auto& n1 = node.as_table().at("str1");
    ASSERT_EQ(n1.as_string(), value);
    const auto& n2 = node.as_table().at("str2");
    ASSERT_EQ(n2.as_string(), value);
    const auto& n3 = node.as_table().at("str3");
    ASSERT_EQ(n3.as_string(), value);
}

TEST(String, string4) {
    std::string path = TEST_CASE_DIR "/string4.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    std::string str4("Here are two quotation marks: \"\". Simple enough.");
    const auto& n1 = node.as_table().at("str4");
    ASSERT_EQ(n1.as_string(), str4);

    std::string str5("Here are three quotation marks: \"\"\".");
    const auto& n2 = node.as_table().at("str5");
    ASSERT_EQ(n2.as_string(), str5);

    std::string str6("Here are fifteen quotation marks: \"\"\"\"\"\"\"\"\"\"\"\"\"\"\".");
    const auto& n3 = node.as_table().at("str6");
    ASSERT_EQ(n3.as_string(), str6);

    std::string str7("\"This,\" she said, \"is just a pointless statement.\"");
    const auto& n4 = node.as_table().at("str7");
    ASSERT_EQ(n4.as_string(), str7);
}

TEST(String, string5) {
    std::string path = TEST_CASE_DIR "/string5.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("winpath");
    ASSERT_EQ(n1.as_string(), std::string(R"(C:\Users\nodejs\templates)"));
    const auto& n2 = node.as_table().at("winpath2");
    ASSERT_EQ(n2.as_string(), std::string(R"(\\ServerX\admin$\system32\)"));
    const auto& n3 = node.as_table().at("quoted");
    ASSERT_EQ(n3.as_string(), std::string(R"(Tom "Dubs" Preston-Werner)"));
    const auto& n4 = node.as_table().at("regex");
    ASSERT_EQ(n4.as_string(), std::string(R"(<\i\c*\s*>)"));
}

TEST(String, string6) {
    std::string path = TEST_CASE_DIR "/string6.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("regex2");
    ASSERT_EQ(n1.as_string(), std::string(R"(I [dw]on't need \d{2} apples)"));
    const auto& n2 = node.as_table().at("lines");
    ASSERT_EQ(n2.as_string(),
              std::string("The first newline is\ntrimmed in raw strings.\n   All other "
                          "whitespace\n   is preserved.\n"));
}

TEST(String, string7) {
    std::string path = TEST_CASE_DIR "/string7.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("quot15");
    ASSERT_EQ(n1.as_string(),
              std::string(R"(Here are fifteen quotation marks: """"""""""""""")"));
    const auto& n2 = node.as_table().at("str");
    ASSERT_EQ(n2.as_string(),
              std::string(R"('That,' she said, 'is still pointless.')"));
}

TEST(String, string8) {
    std::string path = TEST_CASE_DIR "/string8.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("apos15");
    ASSERT_EQ(n1.as_string(),
              std::string(R"(Here are fifteen apostrophes: ''''''''''''''')"));
}


RUN_ALL_TESTS()
