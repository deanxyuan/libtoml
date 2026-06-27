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
#include <limits>

TEST(Writer, WriteNull) {
    TOML::Value v;
    // Null produces empty string for to_toml, "null" for to_json
    std::string toml = TOML::to_toml(v);
    std::string json = TOML::to_json(v);
    ASSERT_TRUE(toml.empty());
    ASSERT_EQ(json, "null");
}

TEST(Writer, WriteBoolean) {
    TOML::Value v_true(true);
    ASSERT_EQ(TOML::to_toml(v_true), "true");
    ASSERT_EQ(TOML::to_json(v_true), "true");

    TOML::Value v_false(false);
    ASSERT_EQ(TOML::to_toml(v_false), "false");
    ASSERT_EQ(TOML::to_json(v_false), "false");
}

TEST(Writer, WriteInteger) {
    TOML::Value v(42);
    ASSERT_EQ(TOML::to_toml(v), "42");
    ASSERT_EQ(TOML::to_json(v), "42");

    TOML::Value v_neg(-100);
    ASSERT_EQ(TOML::to_toml(v_neg), "-100");
    ASSERT_EQ(TOML::to_json(v_neg), "-100");
}

TEST(Writer, WriteFloat) {
    TOML::Value v(3.14);
    std::string result = TOML::to_toml(v);
    // Float formatting uses full precision; must contain digits and decimal point
    ASSERT_TRUE(result.find("3.14") == 0) << "got: " << result;
    ASSERT_TRUE(result.find('.') != std::string::npos);

    TOML::Value v_neg(-2.5);
    ASSERT_EQ(TOML::to_toml(v_neg), "-2.5");
    ASSERT_EQ(TOML::to_json(v_neg), "-2.5");

    // inf/nan
    TOML::Value v_inf(std::numeric_limits<double>::infinity());
    ASSERT_EQ(TOML::to_toml(v_inf), "inf");
    ASSERT_EQ(TOML::to_json(v_inf), "null");

    TOML::Value v_nan(std::numeric_limits<double>::quiet_NaN());
    ASSERT_EQ(TOML::to_toml(v_nan), "nan");
    ASSERT_EQ(TOML::to_json(v_nan), "null");
}

TEST(Writer, WriteString) {
    TOML::Value v(std::string("hello world"));
    ASSERT_EQ(TOML::to_toml(v), "\"hello world\"");
    ASSERT_EQ(TOML::to_json(v), "\"hello world\"");

    // Special characters: newline and tab are escaped
    TOML::Value v_special(std::string("line1\nline2\ttab"));
    std::string result = TOML::to_toml(v_special);
    ASSERT_TRUE(result.find("\\n") != std::string::npos);
    ASSERT_TRUE(result.find("\\t") != std::string::npos);
    ASSERT_EQ(TOML::to_json(v_special), "\"line1\\nline2\\ttab\"");
}

TEST(Writer, WriteArray) {
    TOML::Array arr;
    arr.push_back(TOML::Value(1));
    arr.push_back(TOML::Value(2));
    arr.push_back(TOML::Value(3));

    TOML::Value v(std::move(arr));
    ASSERT_EQ(TOML::to_toml(v), "[1, 2, 3]");
    ASSERT_EQ(TOML::to_json(v), "[1, 2, 3]");

    // String array
    TOML::Array arr2;
    arr2.push_back(TOML::Value(std::string("a")));
    arr2.push_back(TOML::Value(std::string("b")));
    ASSERT_EQ(arr2.to_toml(), "[\"a\", \"b\"]");
    ASSERT_EQ(arr2.to_json(), "[\"a\", \"b\"]");

    // Empty array
    TOML::Array empty;
    ASSERT_EQ(empty.to_toml(), "[]");
    ASSERT_EQ(empty.to_json(), "[]");
}

TEST(Writer, WriteTable) {
    TOML::Table tbl;
    tbl.insert("name", TOML::Value(std::string("Alice")));
    tbl.insert("age", TOML::Value(30));

    // Table to_toml produces key = value lines (order unspecified)
    std::string toml = tbl.to_toml();
    ASSERT_TRUE(toml.find("name = \"Alice\"") != std::string::npos);
    ASSERT_TRUE(toml.find("age = 30") != std::string::npos);

    // Table to_json
    std::string json = tbl.to_json();
    ASSERT_TRUE(json.find("\"name\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"Alice\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"age\"") != std::string::npos);
    ASSERT_TRUE(json.find(": 30") != std::string::npos);
    ASSERT_TRUE(json.front() == '{');
    ASSERT_TRUE(json.back() == '}');

    // Value wrapping table
    TOML::Value v(std::move(tbl));
    std::string toml2 = TOML::to_toml(v);
    ASSERT_TRUE(toml2.find("name = \"Alice\"") != std::string::npos);
    ASSERT_TRUE(toml2.find("age = 30") != std::string::npos);
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

    // to_json: nested JSON is well-formed
    std::string json = root.to_json();
    ASSERT_TRUE(json.find("\"server\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"host\"") != std::string::npos);
    ASSERT_TRUE(json.find("\"localhost\"") != std::string::npos);
    ASSERT_TRUE(json.find("8080") != std::string::npos);
    ASSERT_TRUE(json.find("8181") != std::string::npos);

    // Formatted writer with sorted keys
    TOML::FormatOptions opts;
    opts.indent = 2;
    opts.sorted_keys = true;
    TOML::Value root_val(std::move(root));
    std::string formatted = TOML::to_toml(root_val, opts);
    ASSERT_FALSE(formatted.empty());
    ASSERT_TRUE(formatted.find("server") != std::string::npos);
    ASSERT_TRUE(formatted.find("localhost") != std::string::npos);
    ASSERT_TRUE(formatted.find("8080") != std::string::npos);
}

TEST(Writer, FormattedWriter) {
    // Parse a complex document and format with options
    std::string input = R"(
name = "test"
port = 8080
enabled = true
ratio = 3.14

[server]
host = "localhost"
port = 8080

[[items]]
id = 1

[[items]]
id = 2
)";
    auto result = TOML::parse_string(input);
    ASSERT_TRUE(result.ok()) << result.error.to_string();

    TOML::FormatOptions opts;
    opts.sorted_keys = true;
    std::string formatted = TOML::to_toml(result.value, opts);
    ASSERT_FALSE(formatted.empty());

    // Should contain all key-value pairs
    ASSERT_TRUE(formatted.find("name = \"test\"") != std::string::npos);
    ASSERT_TRUE(formatted.find("port = 8080") != std::string::npos);
    ASSERT_TRUE(formatted.find("enabled = true") != std::string::npos);
    ASSERT_TRUE(formatted.find("ratio = 3.14") != std::string::npos);

    // Server section (size <= 3, rendered inline)
    ASSERT_TRUE(formatted.find("server") != std::string::npos);
    ASSERT_TRUE(formatted.find("localhost") != std::string::npos);

    // Array of tables uses [[section]] syntax
    ASSERT_TRUE(formatted.find("[[items]]") != std::string::npos);
    ASSERT_TRUE(formatted.find("id = 1") != std::string::npos);
    ASSERT_TRUE(formatted.find("id = 2") != std::string::npos);
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
    ASSERT_TRUE(test::CheckTableHasStringValue(tbl, "name", "test"));
    ASSERT_TRUE(test::CheckTableHasIntValue(tbl, "port", 8080));
    ASSERT_TRUE(test::CheckTableHasBoolValue(tbl, "enabled", true));
    ASSERT_TRUE(test::CheckTableHasFloatValue(tbl, "ratio", 3.14));
}

TEST(Writer, RoundTripArray) {
    std::string input = "arr = [1, 2, 3]\n";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    std::string written = TOML::to_toml(r1.value);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& arr = r2.value.as_table().at("arr").as_array();
    ASSERT_EQ(arr.size(), 3u);
    ASSERT_EQ(arr.at(0).as_integer(), 1);
    ASSERT_EQ(arr.at(1).as_integer(), 2);
    ASSERT_EQ(arr.at(2).as_integer(), 3);
}

TEST(Writer, RoundTripNestedArray) {
    std::string input = "matrix = [[1, 2], [3, 4]]\n";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    std::string written = TOML::to_toml(r1.value);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& matrix = r2.value.as_table().at("matrix").as_array();
    ASSERT_EQ(matrix.size(), 2u);
    ASSERT_EQ(matrix.at(0).as_array().at(0).as_integer(), 1);
    ASSERT_EQ(matrix.at(0).as_array().at(1).as_integer(), 2);
    ASSERT_EQ(matrix.at(1).as_array().at(0).as_integer(), 3);
    ASSERT_EQ(matrix.at(1).as_array().at(1).as_integer(), 4);
}

TEST(Writer, RoundTripDateTime) {
    std::string input = "dt = 2023-06-15T14:30:45\n";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    std::string written = TOML::to_toml(r1.value);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& dt = r2.value.as_table().at("dt").as_datetime();
    ASSERT_EQ(dt.year, 2023);
    ASSERT_EQ(dt.month, 6);
    ASSERT_EQ(dt.day, 15);
    ASSERT_EQ(dt.hour, 14);
    ASSERT_EQ(dt.minute, 30);
    ASSERT_EQ(dt.second, 45);
}

TEST(Writer, RoundTripTable) {
    // Table with >3 entries triggers section format in formatted writer
    std::string input = R"(
[server]
host = "localhost"
port = 8080
protocol = "https"
timeout = 30
)";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    // Use formatted writer for round-trip
    TOML::FormatOptions opts;
    opts.sorted_keys = true;
    std::string written = TOML::to_toml(r1.value, opts);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& server = r2.value.as_table().at("server").as_table();
    ASSERT_TRUE(test::CheckTableHasStringValue(server, "host", "localhost"));
    ASSERT_TRUE(test::CheckTableHasIntValue(server, "port", 8080));
    ASSERT_TRUE(test::CheckTableHasStringValue(server, "protocol", "https"));
    ASSERT_TRUE(test::CheckTableHasIntValue(server, "timeout", 30));
}

TEST(Writer, RoundTripArrayOfTables) {
    // Array of tables: [[fruits]] format
    std::string input = R"(
[[fruits]]
name = "apple"
color = "red"

[[fruits]]
name = "banana"
color = "yellow"
)";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    // Use formatted writer which handles [[section]] syntax
    TOML::FormatOptions opts;
    opts.sorted_keys = true;
    std::string written = TOML::to_toml(r1.value, opts);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& fruits = r2.value.as_table().at("fruits").as_array();
    ASSERT_EQ(fruits.size(), 2u);
    ASSERT_TRUE(test::CheckTableHasStringValue(fruits.at(0).as_table(), "name", "apple"));
    ASSERT_TRUE(test::CheckTableHasStringValue(fruits.at(0).as_table(), "color", "red"));
    ASSERT_TRUE(test::CheckTableHasStringValue(fruits.at(1).as_table(), "name", "banana"));
    ASSERT_TRUE(test::CheckTableHasStringValue(fruits.at(1).as_table(), "color", "yellow"));
}

TEST(Writer, RoundTripCompound) {
    // Compound document with multiple types
    std::string input = R"(
title = "Test"
count = 42
ratio = 2.5
enabled = true
tags = ["a", "b", "c"]

[owner]
name = "Admin"

[[items]]
id = 1
value = "first"

[[items]]
id = 2
value = "second"
)";
    auto r1 = TOML::parse_string(input);
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();

    TOML::FormatOptions opts;
    opts.sorted_keys = true;
    std::string written = TOML::to_toml(r1.value, opts);
    auto r2 = TOML::parse_string(written);
    ASSERT_TRUE(r2.ok()) << "Round-trip parse failed: " << r2.error.to_string();

    const auto& tbl = r2.value.as_table();
    ASSERT_TRUE(test::CheckTableHasStringValue(tbl, "title", "Test"));
    ASSERT_TRUE(test::CheckTableHasIntValue(tbl, "count", 42));
    ASSERT_TRUE(test::CheckTableHasFloatValue(tbl, "ratio", 2.5));
    ASSERT_TRUE(test::CheckTableHasBoolValue(tbl, "enabled", true));

    const auto& tags = tbl.at("tags").as_array();
    ASSERT_EQ(tags.size(), 3u);
    ASSERT_EQ(tags.at(0).as_string(), "a");
    ASSERT_EQ(tags.at(1).as_string(), "b");
    ASSERT_EQ(tags.at(2).as_string(), "c");

    const auto& owner = tbl.at("owner").as_table();
    ASSERT_TRUE(test::CheckTableHasStringValue(owner, "name", "Admin"));

    const auto& items = tbl.at("items").as_array();
    ASSERT_EQ(items.size(), 2u);
    ASSERT_TRUE(test::CheckTableHasIntValue(items.at(0).as_table(), "id", 1));
    ASSERT_TRUE(test::CheckTableHasStringValue(items.at(0).as_table(), "value", "first"));
    ASSERT_TRUE(test::CheckTableHasIntValue(items.at(1).as_table(), "id", 2));
    ASSERT_TRUE(test::CheckTableHasStringValue(items.at(1).as_table(), "value", "second"));
}

RUN_ALL_TESTS()
