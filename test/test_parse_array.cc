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

TEST(Array, BasicTest) {
    std::string path = TEST_CASE_DIR "/arr1.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("integers");
    ASSERT_EQ(n1.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasIntValue(n1.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n1.as_array(), 1, 2));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n1.as_array(), 2, 3));

    const auto& n2 = node.as_table().at("colors");
    ASSERT_EQ(n2.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasStringValue(n2.as_array(), 0, "red"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(n2.as_array(), 1, "yellow"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(n2.as_array(), 2, "green"));

    const auto& nested_arrays_of_ints = node.as_table().at("nested_arrays_of_ints");
    ASSERT_EQ(nested_arrays_of_ints.as_array().size(), 2);
    const auto& n3n1 = nested_arrays_of_ints.as_array().at(0);
    ASSERT_EQ(n3n1.as_array().size(), 2);
    ASSERT_TRUE(test::CheckArrayHasIntValue(n3n1.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n3n1.as_array(), 1, 2));
    const auto& n3n2 = nested_arrays_of_ints.as_array().at(1);
    ASSERT_EQ(n3n2.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasIntValue(n3n2.as_array(), 0, 3));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n3n2.as_array(), 1, 4));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n3n2.as_array(), 2, 5));

    const auto& nested_mixed_array = node.as_table().at("nested_mixed_array");
    ASSERT_EQ(nested_mixed_array.as_array().size(), 2);
    const auto& n4n1 = nested_mixed_array.as_array().at(0);
    ASSERT_EQ(n4n1.as_array().size(), 2);
    ASSERT_TRUE(test::CheckArrayHasIntValue(n4n1.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(n4n1.as_array(), 1, 2));
    const auto& n4n2 = nested_mixed_array.as_array().at(1);
    ASSERT_EQ(n4n2.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasStringValue(n4n2.as_array(), 0, "a"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(n4n2.as_array(), 1, "b"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(n4n2.as_array(), 2, "c"));

    const auto& string_array = node.as_table().at("string_array");
    ASSERT_EQ(string_array.as_array().size(), 4);
    ASSERT_TRUE(test::CheckArrayHasStringValue(string_array.as_array(), 0, "all"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(string_array.as_array(), 1, "strings"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(string_array.as_array(), 2, "are the same"));
    ASSERT_TRUE(test::CheckArrayHasStringValue(string_array.as_array(), 3, "type"));

    const auto& numbers = node.as_table().at("numbers");
    ASSERT_EQ(numbers.as_array().size(), 6);
    const auto& f1 = numbers.as_array().at(0);
    ASSERT_FLOAT_EQ(f1.as_float(), 0.1);

    const auto& f2 = numbers.as_array().at(1);
    ASSERT_FLOAT_EQ(f2.as_float(), 0.2);

    const auto& f3 = numbers.as_array().at(2);
    ASSERT_FLOAT_EQ(f3.as_float(), 0.5);

    ASSERT_TRUE(test::CheckArrayHasIntValue(numbers.as_array(), 3, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(numbers.as_array(), 4, 2));
    ASSERT_TRUE(test::CheckArrayHasIntValue(numbers.as_array(), 5, 5));

    const auto& contributors = node.as_table().at("contributors");
    ASSERT_EQ(contributors.as_array().size(), 2);
    ASSERT_TRUE(test::CheckArrayHasStringValue(contributors.as_array(), 0, "Foo Bar <foo@example.com>"));
    const auto& n5n2 = contributors.as_array().at(1);
    ASSERT_EQ(n5n2.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(n5n2.as_table(), "name", "Baz Qux"));
    ASSERT_TRUE(test::CheckTableHasStringValue(n5n2.as_table(), "email", "bazqux@example.com"));
    ASSERT_TRUE(test::CheckTableHasStringValue(n5n2.as_table(), "url", "https://example.com/bazqux"));
}

TEST(Array, MultiLineTest) {
    std::string path = TEST_CASE_DIR "/arr2.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& integers2 = node.as_table().at("integers2");
    ASSERT_EQ(integers2.as_array().size(), 3);
    ASSERT_TRUE(test::CheckArrayHasIntValue(integers2.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(integers2.as_array(), 1, 2));
    ASSERT_TRUE(test::CheckArrayHasIntValue(integers2.as_array(), 2, 3));

    const auto& integers3 = node.as_table().at("integers3");
    ASSERT_EQ(integers3.as_array().size(), 2);
    ASSERT_TRUE(test::CheckArrayHasIntValue(integers3.as_array(), 0, 1));
    ASSERT_TRUE(test::CheckArrayHasIntValue(integers3.as_array(), 1, 2));
}


RUN_ALL_TESTS()
