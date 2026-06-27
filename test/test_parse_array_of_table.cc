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

TEST(Array, arrtab1) {
    std::string path = TEST_CASE_DIR "/arrtab1.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& products = node.as_table().at("products");
    ASSERT_TRUE(products.is_array());
    ASSERT_EQ(products.as_array().size(), 3);

    const auto& n1 = products.as_array().at(0);
    ASSERT_EQ(n1.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(n1.as_table(), "name", "Hammer"));
    ASSERT_TRUE(test::CheckTableHasIntValue(n1.as_table(), "sku", 738594937));

    const auto& n2 = products.as_array().at(1);
    ASSERT_TRUE(n2.is_table());
    ASSERT_EQ(n2.as_table().size(), 0);

    const auto& n3 = products.as_array().at(2);
    ASSERT_EQ(n3.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(n3.as_table(), "name", "Nail"));
    ASSERT_TRUE(test::CheckTableHasIntValue(n3.as_table(), "sku", 284758393));
    ASSERT_TRUE(test::CheckTableHasStringValue(n3.as_table(), "color", "gray"));
}

TEST(Array, arrtab2) {
    std::string path = TEST_CASE_DIR "/arrtab2.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& fruits = node.as_table().at("fruits");
    ASSERT_TRUE(fruits.is_array());
    ASSERT_EQ(fruits.as_array().size(), 2);

    const auto& n1 = fruits.as_array().at(0);
    ASSERT_EQ(n1.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(n1.as_table(), "name", "apple"));
    const auto& physical = n1.as_table().at("physical");
    ASSERT_TRUE(test::CheckTableHasStringValue(physical.as_table(), "color", "red"));
    ASSERT_TRUE(test::CheckTableHasStringValue(physical.as_table(), "shape", "round"));
    const auto& varieties = n1.as_table().at("varieties");
    ASSERT_TRUE(varieties.is_array());
    ASSERT_EQ(varieties.as_array().size(), 2);
    const auto& e1 = varieties.as_array().at(0);
    const auto& e2 = varieties.as_array().at(1);
    ASSERT_TRUE(test::CheckTableHasStringValue(e1.as_table(), "name", "red delicious"));
    ASSERT_TRUE(test::CheckTableHasStringValue(e2.as_table(), "name", "granny smith"));

    const auto& n2 = fruits.as_array().at(1);
    ASSERT_TRUE(n2.is_table());
    ASSERT_EQ(n2.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(n2.as_table(), "name", "banana"));

    const auto& varieties2 = n2.as_table().at("varieties");
    ASSERT_TRUE(varieties2.is_array());
    ASSERT_EQ(varieties2.as_array().size(), 1);
    const auto& e3 = varieties2.as_array().at(0);
    ASSERT_TRUE(test::CheckTableHasStringValue(e3.as_table(), "name", "plantain"));
}

TEST(Array, arrtab3) {
    std::string path = TEST_CASE_DIR "/arrtab3.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Array, arrtab4) {
    std::string path = TEST_CASE_DIR "/arrtab4.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Array, arrtab5) {
    std::string path = TEST_CASE_DIR "/arrtab5.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Array, arrtab6) {
    std::string path = TEST_CASE_DIR "/arrtab6.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Array, arrtab7) {
    std::string path = TEST_CASE_DIR "/arrtab7.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& points = node.as_table().at("points");
    ASSERT_EQ(points.as_array().size(), 3);

    const auto& n1 = points.as_array().at(0);
    ASSERT_TRUE(n1.is_table());
    ASSERT_EQ(n1.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasIntValue(n1.as_table(), "x", 1));
    ASSERT_TRUE(test::CheckTableHasIntValue(n1.as_table(), "y", 2));
    ASSERT_TRUE(test::CheckTableHasIntValue(n1.as_table(), "z", 3));

    const auto& n2 = points.as_array().at(1);
    ASSERT_TRUE(n2.is_table());
    ASSERT_EQ(n2.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasIntValue(n2.as_table(), "x", 7));
    ASSERT_TRUE(test::CheckTableHasIntValue(n2.as_table(), "y", 8));
    ASSERT_TRUE(test::CheckTableHasIntValue(n2.as_table(), "z", 9));

    const auto& n3 = points.as_array().at(2);
    ASSERT_TRUE(n3.is_table());
    ASSERT_EQ(n3.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasIntValue(n3.as_table(), "x", 2));
    ASSERT_TRUE(test::CheckTableHasIntValue(n3.as_table(), "y", 4));
    ASSERT_TRUE(test::CheckTableHasIntValue(n3.as_table(), "z", 8));
}


RUN_ALL_TESTS()
