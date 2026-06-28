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

TEST(InlineTable, inline_table) {
    std::string path = TEST_CASE_DIR "/inline_table.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& x = node.as_table().at("x");
    ASSERT_EQ(x.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasIntValue(x.as_table(), "a", 1));
    ASSERT_TRUE(test::CheckTableHasIntValue(x.as_table(), "b", 2));
}

TEST(InlineTable, inlinetab1) {
    std::string path = TEST_CASE_DIR "/inlinetab1.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& name = node.as_table().at("name");
    ASSERT_EQ(name.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(name.as_table(), "first", "Tom"));
    ASSERT_TRUE(test::CheckTableHasStringValue(name.as_table(), "last", "Preston-Werner"));

    const auto& point = node.as_table().at("point");
    ASSERT_EQ(point.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasIntValue(point.as_table(), "x", 1));
    ASSERT_TRUE(test::CheckTableHasIntValue(point.as_table(), "y", 2));

    // animal = { type.name = "pug" }
    const auto& animal = node.as_table().at("animal");
    ASSERT_EQ(animal.as_table().size(), 1);
    const auto& sub = animal.as_table().at("type");
    ASSERT_EQ(sub.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasStringValue(sub.as_table(), "name", "pug"));
}

TEST(InlineTable, inlinetab2) {
    std::string path = TEST_CASE_DIR "/inlinetab2.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(InlineTable, inlinetab3) {
    std::string path = TEST_CASE_DIR "/inlinetab3.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}


RUN_ALL_TESTS()
