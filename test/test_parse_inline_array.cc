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

TEST(InlineTable, array_of_tables) {
    std::string path = TEST_CASE_DIR "/array_of_tables.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& x = node.as_table().at("x");
    ASSERT_TRUE(x.is_array());
    ASSERT_EQ(x.as_array().size(), 2);
    const auto& N1 = x.as_array().at(0);
    ASSERT_TRUE(N1.is_table());
    ASSERT_TRUE(test::CheckTableHasIntValue(N1.as_table(), "a", 1));

    const auto& N2 = x.as_array().at(1);
    ASSERT_TRUE(N2.is_table());
    ASSERT_TRUE(test::CheckTableHasIntValue(N2.as_table(), "a", 2));
}

TEST(InlineTable, inline_array) {
    std::string path = TEST_CASE_DIR "/inline_array.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& x = node.as_table().at("x");
    ASSERT_TRUE(x.is_array());
    ASSERT_EQ(x.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasIntValue(x.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(x.as_array(), 1, 2));
    ASSERT_TRUE(test::CheckArrayHasIntValue(x.as_array(), 2, 3));
}


RUN_ALL_TESTS()
