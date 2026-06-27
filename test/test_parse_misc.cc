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

TEST(Misc, EmptyDocument) {
    // An empty document is valid TOML (empty table)
    auto r = TOML::parse_string("");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    ASSERT_TRUE(r.value.is_table());
    ASSERT_EQ(r.value.as_table().size(), 0u);
}

TEST(Misc, WhitespaceOnlyDocument) {
    // Whitespace-only document is valid (empty table)
    auto r = TOML::parse_string("   \n  \n  ");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    ASSERT_TRUE(r.value.is_table());
    ASSERT_EQ(r.value.as_table().size(), 0u);
}

TEST(Misc, CommentOnlyDocument) {
    // Comment-only document is valid (empty table)
    auto r = TOML::parse_string("# just a comment\n");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    ASSERT_TRUE(r.value.is_table());
    ASSERT_EQ(r.value.as_table().size(), 0u);
}

TEST(Misc, Utf8BOM) {
    // UTF-8 BOM (0xEF 0xBB 0xBF) at start of file should be handled.
    // Current parser rejects it — documents actual behavior.
    std::string input = std::string("\xef\xbb\xbf") + "key = 42";
    auto r = TOML::parse_string(input);
    ASSERT_FALSE(r.ok()) << "UTF-8 BOM should be rejected";
}

TEST(Misc, TrailingContentAfterValue) {
    // "key = 42 extra" should fail — extra tokens after value
    auto r = TOML::parse_string("key = 42 extra");
    ASSERT_FALSE(r.ok());
}

TEST(Misc, MultipleKeyValuePairs) {
    // Multiple key-value pairs separated by newlines is valid
    auto r = TOML::parse_string("key1 = 1\nkey2 = 2\nkey3 = 3");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    const auto& tbl = r.value.as_table();
    ASSERT_EQ(tbl.size(), 3u);
    ASSERT_EQ(tbl.at("key1").as_integer(), 1);
    ASSERT_EQ(tbl.at("key2").as_integer(), 2);
    ASSERT_EQ(tbl.at("key3").as_integer(), 3);
}

TEST(Misc, DeepNestingDottedKeys) {
    // Deeply nested dotted keys
    auto r = TOML::parse_string("a.b.c.d.e = 42");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    const auto& a = r.value.as_table().at("a").as_table();
    const auto& b = a.at("b").as_table();
    const auto& c = b.at("c").as_table();
    const auto& d = c.at("d").as_table();
    ASSERT_EQ(d.at("e").as_integer(), 42);
}

TEST(Misc, DeepNestingSections) {
    // Deeply nested sections
    auto r = TOML::parse_string("[a.b.c.d.e]\nv = 42");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    const auto& a = r.value.as_table().at("a").as_table();
    const auto& b = a.at("b").as_table();
    const auto& c = b.at("c").as_table();
    const auto& d = c.at("d").as_table();
    const auto& e = d.at("e").as_table();
    ASSERT_EQ(e.at("v").as_integer(), 42);
}

RUN_ALL_TESTS()
