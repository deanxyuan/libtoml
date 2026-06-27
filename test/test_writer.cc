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

TEST(Writer, WriteNull) {
    TOML::Value v;
    std::string result = TOML::to_toml(v);
    // Null values may produce empty string or a placeholder
    // The exact format depends on implementation, but should not throw
    ASSERT_NO_THROW(TOML::to_toml(v));
    ASSERT_NO_THROW(TOML::to_json(v));
}

TEST(Writer, WriteBoolean) {
    TOML::Value v_true(true);
    std::string result_true = TOML::to_toml(v_true);
    ASSERT_FALSE(result_true.empty());

    TOML::Value v_false(false);
    std::string result_false = TOML::to_toml(v_false);
    ASSERT_FALSE(result_false.empty());

    // to_json
    ASSERT_NO_THROW(TOML::to_json(v_true));
    ASSERT_NO_THROW(TOML::to_json(v_false));
}

TEST(Writer, WriteInteger) {
    TOML::Value v(42);
    std::string result = TOML::to_toml(v);
    ASSERT_FALSE(result.empty());

    TOML::Value v_neg(-100);
    std::string result_neg = TOML::to_toml(v_neg);
    ASSERT_FALSE(result_neg.empty());

    // to_json
    ASSERT_NO_THROW(TOML::to_json(v));
}

TEST(Writer, WriteFloat) {
    TOML::Value v(3.14);
    std::string result = TOML::to_toml(v);
    ASSERT_FALSE(result.empty());

    TOML::Value v_neg(-2.5);
    std::string result_neg = TOML::to_toml(v_neg);
    ASSERT_FALSE(result_neg.empty());

    // to_json
    ASSERT_NO_THROW(TOML::to_json(v));
}

TEST(Writer, WriteString) {
    TOML::Value v(std::string("hello world"));
    std::string result = TOML::to_toml(v);
    ASSERT_FALSE(result.empty());

    // Special characters
    TOML::Value v_special(std::string("line1\nline2\ttab"));
    std::string result_special = TOML::to_toml(v_special);
    ASSERT_FALSE(result_special.empty());

    // to_json
    ASSERT_NO_THROW(TOML::to_json(v));
}

TEST(Writer, WriteArray) {
    TOML::Array arr;
    arr.push_back(TOML::Value(1));
    arr.push_back(TOML::Value(2));
    arr.push_back(TOML::Value(3));

    TOML::Value v(std::move(arr));
    std::string result = TOML::to_toml(v);
    ASSERT_FALSE(result.empty());

    // Array to_toml and to_json
    TOML::Array arr2;
    arr2.push_back(TOML::Value(std::string("a")));
    arr2.push_back(TOML::Value(std::string("b")));
    std::string result2 = arr2.to_toml();
    ASSERT_FALSE(result2.empty());
    std::string json2 = arr2.to_json();
    ASSERT_FALSE(json2.empty());
}

TEST(Writer, WriteTable) {
    TOML::Table tbl;
    tbl.insert("name", TOML::Value(std::string("Alice")));
    tbl.insert("age", TOML::Value(30));

    // Table to_toml
    std::string result = tbl.to_toml();
    ASSERT_FALSE(result.empty());

    // Table to_json
    std::string json = tbl.to_json();
    ASSERT_FALSE(json.empty());

    // Value wrapping table
    TOML::Value v(std::move(tbl));
    std::string result2 = TOML::to_toml(v);
    ASSERT_FALSE(result2.empty());
}

TEST(Writer, WriteNested) {
    // Create a nested structure: { "server": { "host": "localhost", "ports":
    // [8080, 8181] } }
    TOML::Array ports;
    ports.push_back(TOML::Value(8080));
    ports.push_back(TOML::Value(8181));

    TOML::Table server;
    server.insert("host", TOML::Value(std::string("localhost")));
    server.insert("ports", TOML::Value(std::move(ports)));

    TOML::Table root;
    root.insert("server", TOML::Value(std::move(server)));

    std::string result = root.to_toml();
    ASSERT_FALSE(result.empty());

    std::string json = root.to_json();
    ASSERT_FALSE(json.empty());

    // With format options
    TOML::FormatOptions opts;
    opts.indent = 2;
    opts.sorted_keys = true;
    TOML::Value root_val(std::move(root));
    std::string formatted = TOML::to_toml(root_val, opts);
    ASSERT_FALSE(formatted.empty());
}

TEST(Writer, RoundTrip) {
    // Parse a TOML string, write it back, parse again, and verify values match
    std::string input = R"(
name = "test"
port = 8080
enabled = true
ratio = 3.14
)";

    auto result1 = TOML::parse_string(input);
    ASSERT_TRUE(result1.ok()) << result1.error.to_string();

    // Write back to TOML
    std::string written = TOML::to_toml(result1.value);
    ASSERT_FALSE(written.empty());

    // Parse the written output
    auto result2 = TOML::parse_string(written);
    ASSERT_TRUE(result2.ok()) << "Round-trip parse failed: "
                              << result2.error.to_string();

    // Verify values survive the round trip
    const auto& tbl = result2.value.as_table();
    ASSERT_TRUE(testutil::CheckTableHasStringValue(tbl, "name", "test"));
    ASSERT_TRUE(testutil::CheckTableHasIntValue(tbl, "port", 8080));
    ASSERT_TRUE(testutil::CheckTableHasBoolValue(tbl, "enabled", true));
    ASSERT_TRUE(testutil::CheckTableHasFloatValue(tbl, "ratio", 3.14));
}

RUN_ALL_TESTS()
