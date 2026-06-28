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

#include "util/testutil.h"

TEST(Array, EmptyArray) {
    toml::Array arr;
    ASSERT_TRUE(arr.empty());
    ASSERT_EQ(arr.size(), 0u);
    ASSERT_EQ(arr.begin(), arr.end());
}

TEST(Array, PushBackAndAccess) {
    toml::Array arr;
    arr.push_back(toml::Value(1));
    arr.push_back(toml::Value(std::string("hello")));
    arr.push_back(toml::Value(true));

    ASSERT_FALSE(arr.empty());
    ASSERT_EQ(arr.size(), 3u);

    // at()
    ASSERT_TRUE(arr.at(0).is_integer());
    ASSERT_EQ(arr.at(0).as_integer(), 1);
    ASSERT_TRUE(arr.at(1).is_string());
    ASSERT_EQ(arr.at(1).as_string(), "hello");
    ASSERT_TRUE(arr.at(2).is_boolean());
    ASSERT_TRUE(arr.at(2).as_bool());

    // front() and back()
    ASSERT_EQ(arr.front().as_integer(), 1);
    ASSERT_TRUE(arr.back().as_bool());

    // const access
    const auto& const_arr = arr;
    ASSERT_EQ(const_arr.at(0).as_integer(), 1);
    ASSERT_EQ(const_arr.front().as_integer(), 1);
    ASSERT_TRUE(const_arr.back().as_bool());

    // pop_back
    arr.pop_back();
    ASSERT_EQ(arr.size(), 2u);
}

TEST(Array, AtThrows) {
    toml::Array arr;
    arr.push_back(toml::Value(1));

    ASSERT_THROW(arr.at(1), std::out_of_range);
    ASSERT_THROW(arr.at(100), std::out_of_range);

    const auto& const_arr = arr;
    ASSERT_THROW(const_arr.at(1), std::out_of_range);
}

TEST(Array, OperatorBracket) {
    toml::Array arr;
    arr.push_back(toml::Value(10));
    arr.push_back(toml::Value(20));
    arr.push_back(toml::Value(30));

    // Non-const access
    ASSERT_EQ(arr[0].as_integer(), 10);
    ASSERT_EQ(arr[1].as_integer(), 20);
    ASSERT_EQ(arr[2].as_integer(), 30);

    // Const access
    const auto& const_arr = arr;
    ASSERT_EQ(const_arr[0].as_integer(), 10);
    ASSERT_EQ(const_arr[1].as_integer(), 20);
    ASSERT_EQ(const_arr[2].as_integer(), 30);
}

TEST(Array, Iterate) {
    toml::Array arr;
    arr.push_back(toml::Value(1));
    arr.push_back(toml::Value(2));
    arr.push_back(toml::Value(3));

    int count = 0;
    int64_t sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        ASSERT_TRUE(it->is_integer());
        sum += it->as_integer();
        count++;
    }
    ASSERT_EQ(count, 3);
    ASSERT_EQ(sum, 6);

    // Range-based for
    count = 0;
    for (const auto& val : arr) {
        ASSERT_TRUE(val.is_integer());
        count++;
    }
    ASSERT_EQ(count, 3);
}

TEST(Array, NestedArrays) {
    toml::Array outer;
    toml::Array inner1;
    inner1.push_back(toml::Value(1));
    inner1.push_back(toml::Value(2));

    toml::Array inner2;
    inner2.push_back(toml::Value(3));
    inner2.push_back(toml::Value(4));
    inner2.push_back(toml::Value(5));

    outer.push_back(toml::Value(std::move(inner1)));
    outer.push_back(toml::Value(std::move(inner2)));

    ASSERT_EQ(outer.size(), 2u);

    const auto& a1 = outer.at(0).as_array();
    ASSERT_EQ(a1.size(), 2u);
    ASSERT_EQ(a1.at(0).as_integer(), 1);
    ASSERT_EQ(a1.at(1).as_integer(), 2);

    const auto& a2 = outer.at(1).as_array();
    ASSERT_EQ(a2.size(), 3u);
    ASSERT_EQ(a2.at(0).as_integer(), 3);
    ASSERT_EQ(a2.at(1).as_integer(), 4);
    ASSERT_EQ(a2.at(2).as_integer(), 5);
}

TEST(Array, InsertAndErase) {
    toml::Array arr;
    arr.push_back(toml::Value(1));
    arr.push_back(toml::Value(3));

    // Insert at index 1
    arr.insert(1, toml::Value(2));
    ASSERT_EQ(arr.size(), 3u);
    ASSERT_EQ(arr[0].as_integer(), 1);
    ASSERT_EQ(arr[1].as_integer(), 2);
    ASSERT_EQ(arr[2].as_integer(), 3);

    // Erase at index 0
    arr.erase(0);
    ASSERT_EQ(arr.size(), 2u);
    ASSERT_EQ(arr[0].as_integer(), 2);
    ASSERT_EQ(arr[1].as_integer(), 3);

    // Clear
    arr.clear();
    ASSERT_TRUE(arr.empty());
}

RUN_ALL_TESTS()
