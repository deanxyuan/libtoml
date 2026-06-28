/*
 *
 * Copyright 2022-2026 libtoml authors.
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

TEST(Keys, keys00) {
    std::string path = TEST_CASE_DIR "/keys00.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("key");
    ASSERT_EQ(n1.as_string(), std::string("value"));
    const auto& n2 = node.as_table().at("bare_key");
    ASSERT_EQ(n2.as_string(), std::string("value"));
    const auto& n3 = node.as_table().at("bare-key");
    ASSERT_EQ(n3.as_string(), std::string("value"));
    const auto& n4 = node.as_table().at("1234");
    ASSERT_EQ(n4.as_string(), std::string("value"));
}

TEST(Keys, keys01) {
    std::string path = TEST_CASE_DIR "/keys01.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("127.0.0.1");
    ASSERT_EQ(n1.as_string(), std::string("value"));
    const auto& n2 = node.as_table().at("character encoding");
    ASSERT_EQ(n2.as_string(), std::string("value"));
    const auto& n3 = node.as_table().at("ʎǝʞ");
    ASSERT_EQ(n3.as_string(), std::string("value"));
    const auto& n4 = node.as_table().at("key2");
    ASSERT_EQ(n4.as_string(), std::string("value"));
    const auto& n5 = node.as_table().at("quoted \"value\"");
    ASSERT_EQ(n5.as_string(), std::string("value"));
}

TEST(Keys, keys02) {
    std::string path = TEST_CASE_DIR "/keys02.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Keys, keys03) {
    std::string path = TEST_CASE_DIR "/keys03.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("");
    ASSERT_EQ(n1.as_string(), std::string("blank"));
}

TEST(Keys, keys04) {
    std::string path = TEST_CASE_DIR "/keys04.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("name");
    ASSERT_EQ(n1.as_string(), std::string("Orange"));

    // physical.color = "orange"
    // physical.shape = "round"
    const auto& physical = node.as_table().at("physical");
    ASSERT_EQ(physical.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(physical.as_table(), "color", "orange"));
    ASSERT_TRUE(test::CheckTableHasStringValue(physical.as_table(), "shape", "round"));

    // site."google.com" = true
    const auto& site = node.as_table().at("site");
    ASSERT_EQ(site.as_table().size(), 1);
    const auto& v = site.as_table().at("google.com");
    ASSERT_TRUE(v.is_boolean());
    ASSERT_TRUE(v.as_bool());
}

TEST(Keys, keys05) {
    std::string path = TEST_CASE_DIR "/keys05.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& fruit = node.as_table().at("fruit");
    ASSERT_EQ(fruit.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(fruit.as_table(), "color", "yellow"));
    ASSERT_TRUE(test::CheckTableHasStringValue(fruit.as_table(), "name", "banana"));
    ASSERT_TRUE(test::CheckTableHasStringValue(fruit.as_table(), "flavor", "banana"));
}

TEST(Keys, keys06) {
    std::string path = TEST_CASE_DIR "/keys06.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Keys, keys07) {
    std::string path = TEST_CASE_DIR "/keys07.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Keys, keys08) {
    std::string path = TEST_CASE_DIR "/keys08.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& fruit = node.as_table().at("fruit");
    ASSERT_EQ(fruit.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasIntValue(fruit.as_table(), "orange", 2));
    const auto& apple  = fruit.as_table().at("apple");
    const auto& smooth = apple.as_table().at("smooth");
    ASSERT_TRUE(smooth.is_boolean());
    ASSERT_TRUE(smooth.as_bool());
}

TEST(Keys, keys09) {
    std::string path = TEST_CASE_DIR "/keys09.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Keys, keys10) {
    std::string path = TEST_CASE_DIR "/keys10.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& apple = node.as_table().at("apple");
    ASSERT_EQ(apple.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "type", "fruit"));
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "skin", "thin"));
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "color", "red"));

    const auto& orange = node.as_table().at("orange");
    ASSERT_EQ(orange.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "type", "fruit"));
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "skin", "thick"));
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "color", "orange"));
}

TEST(Keys, keys11) {
    std::string path = TEST_CASE_DIR "/keys11.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& apple = node.as_table().at("apple");
    ASSERT_EQ(apple.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "type", "fruit"));
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "skin", "thin"));
    ASSERT_TRUE(test::CheckTableHasStringValue(apple.as_table(), "color", "red"));

    const auto& orange = node.as_table().at("orange");
    ASSERT_EQ(orange.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "type", "fruit"));
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "skin", "thick"));
    ASSERT_TRUE(test::CheckTableHasStringValue(orange.as_table(), "color", "orange"));
}

TEST(Keys, keys12) {
    std::string path = TEST_CASE_DIR "/keys12.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& n1 = node.as_table().at("3");
    ASSERT_EQ(n1.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasStringValue(n1.as_table(), "14159", "pi"));
}


RUN_ALL_TESTS()
