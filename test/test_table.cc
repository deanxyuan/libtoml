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

#include "util/testutil.h"

TEST(Table, EmptyTable) {
    TOML::Table tbl;
    ASSERT_TRUE(tbl.empty());
    ASSERT_EQ(tbl.size(), 0u);
    ASSERT_EQ(tbl.begin(), tbl.end());
}

TEST(Table, InsertAndAccess) {
    TOML::Table tbl;
    tbl.insert("key1", TOML::Value(std::string("value1")));
    tbl.insert("key2", TOML::Value(42));
    tbl.insert("key3", TOML::Value(true));

    ASSERT_FALSE(tbl.empty());
    ASSERT_EQ(tbl.size(), 3u);

    // Access via at()
    ASSERT_TRUE(tbl.at("key1").is_string());
    ASSERT_EQ(tbl.at("key1").as_string(), "value1");
    ASSERT_TRUE(tbl.at("key2").is_integer());
    ASSERT_EQ(tbl.at("key2").as_integer(), 42);
    ASSERT_TRUE(tbl.at("key3").is_boolean());
    ASSERT_TRUE(tbl.at("key3").as_bool());

    // insert_or_assign
    tbl.insert_or_assign("key2", TOML::Value(100));
    ASSERT_EQ(tbl.at("key2").as_integer(), 100);
}

TEST(Table, Contains) {
    TOML::Table tbl;
    tbl.insert("existing", TOML::Value(1));

    ASSERT_TRUE(tbl.contains("existing"));
    ASSERT_FALSE(tbl.contains("nonexistent"));
    ASSERT_EQ(tbl.count("existing"), 1u);
    ASSERT_EQ(tbl.count("nonexistent"), 0u);

    // find
    auto it = tbl.find("existing");
    ASSERT_NE(it, tbl.end());
    ASSERT_EQ(it->first, "existing");

    auto it_end = tbl.find("nonexistent");
    ASSERT_EQ(it_end, tbl.end());
}

TEST(Table, AtThrows) {
    TOML::Table tbl;
    tbl.insert("key", TOML::Value(1));

    ASSERT_THROW(tbl.at("nonexistent"), std::out_of_range);

    const auto& const_tbl = tbl;
    ASSERT_THROW(const_tbl.at("nonexistent"), std::out_of_range);
}

TEST(Table, OperatorBracket) {
    TOML::Table tbl;

    // operator[] creates a null value for missing keys
    auto& v = tbl["new_key"];
    ASSERT_TRUE(v.is_null());
    ASSERT_EQ(tbl.size(), 1u);

    // operator[] on existing key
    tbl.insert("existing", TOML::Value(std::string("hello")));
    auto& v2 = tbl["existing"];
    ASSERT_TRUE(v2.is_string());
    ASSERT_EQ(v2.as_string(), "hello");

    // const operator[]
    const auto& const_tbl = tbl;
    const auto& v3 = const_tbl["existing"];
    ASSERT_TRUE(v3.is_string());
    ASSERT_EQ(v3.as_string(), "hello");
}

TEST(Table, Iterate) {
    TOML::Table tbl;
    tbl.insert("a", TOML::Value(1));
    tbl.insert("b", TOML::Value(2));
    tbl.insert("c", TOML::Value(3));

    int count = 0;
    for (auto it = tbl.begin(); it != tbl.end(); ++it) {
        ASSERT_TRUE(it->second.is_integer());
        count++;
    }
    ASSERT_EQ(count, 3);

    // Range-based for
    count = 0;
    for (const auto& pair : tbl) {
        ASSERT_FALSE(pair.first.empty());
        count++;
    }
    ASSERT_EQ(count, 3);
}

TEST(Table, CopyAndMove) {
    TOML::Table tbl;
    tbl.insert("key", TOML::Value(std::string("value")));

    // Copy
    TOML::Table copy(tbl);
    ASSERT_EQ(copy.size(), 1u);
    ASSERT_EQ(copy.at("key").as_string(), "value");

    // Modify original, copy should be unaffected
    tbl.insert("key2", TOML::Value(42));
    ASSERT_EQ(tbl.size(), 2u);
    ASSERT_EQ(copy.size(), 1u);

    // Move
    TOML::Table moved(std::move(copy));
    ASSERT_EQ(moved.size(), 1u);
    ASSERT_EQ(moved.at("key").as_string(), "value");

    // Erase and clear
    ASSERT_EQ(moved.erase("key"), 1u);
    ASSERT_TRUE(moved.empty());

    tbl.clear();
    ASSERT_TRUE(tbl.empty());
}

RUN_ALL_TESTS()
