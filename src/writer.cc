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

#include "toml/writer.h"
#include "src/format_utils.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <sstream>
#include <vector>

namespace toml {

// ---------------------------------------------------------------------------
// Simple delegation to Value/Table member functions
// ---------------------------------------------------------------------------

std::string to_toml(const Value& value) {
    return value.to_toml();
}

std::string to_toml(const Table& table) {
    return table.to_toml();
}

std::string to_json(const Value& value) {
    return value.to_json();
}

// ---------------------------------------------------------------------------
// Number formatting helpers
// ---------------------------------------------------------------------------

static std::string format_integer(int64_t v) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(v));
    return buf;
}

static std::string format_uinteger(uint64_t v) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(v));
    return buf;
}

// ---------------------------------------------------------------------------
// Helper: make indent string
// ---------------------------------------------------------------------------

static std::string make_indent(size_t level, const FormatOptions& opts) {
    return std::string(level * opts.indent, opts.indent_char);
}

// ---------------------------------------------------------------------------
// Helper: check if a value is a "simple" value (not table/array)
// ---------------------------------------------------------------------------

static bool is_simple_value(const Value& v) {
    return v.type() != kTable && v.type() != kArray;
}

// ---------------------------------------------------------------------------
// Helper: check if an array contains any tables
// ---------------------------------------------------------------------------

static bool array_contains_tables(const Array& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr.at(i).is_table()) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Helper: get sorted keys from a table
// ---------------------------------------------------------------------------

static std::vector<std::string> get_sorted_keys(const Table& table) {
    std::vector<std::string> keys;
    keys.reserve(table.size());
    for (const auto& pair : table) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());
    return keys;
}

// ---------------------------------------------------------------------------
// Helper: get keys (sorted or original order)
// ---------------------------------------------------------------------------

static std::vector<std::string> get_keys(const Table& table,
                                         bool sorted) {
    if (sorted) {
        return get_sorted_keys(table);
    }
    std::vector<std::string> keys;
    keys.reserve(table.size());
    for (const auto& pair : table) {
        keys.push_back(pair.first);
    }
    return keys;
}

// ---------------------------------------------------------------------------
// TOML formatted writer implementation
// ---------------------------------------------------------------------------

// Forward declarations for mutually recursive functions
static void write_toml_value(std::string& out, const Value& value,
                             const FormatOptions& opts, size_t indent_level);
static void write_toml_inline_table(std::string& out, const Table& table);
static void write_toml_inline_array(std::string& out, const Array& arr);

// Write a simple value (not table/array)
static void write_toml_simple(std::string& out, const Value& value) {
    switch (value.type()) {
    case kNull:
        break;
    case kBoolean:
        out += value.as_bool() ? "true" : "false";
        break;
    case kInteger:
        out += format_integer(value.as_integer());
        break;
    case kUInteger:
        out += format_uinteger(value.as_uinteger());
        break;
    case kFloat:
        out += format_float_toml(value.as_float());
        break;
    case kString:
        out += escape_toml_string(value.as_string());
        break;
    case kDateTime:
        out += value.as_datetime().to_toml();
        break;
    default:
        break;
    }
}

// Write an inline array: [1, 2, 3]
static void write_toml_inline_array(std::string& out, const Array& arr) {
    out += '[';
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) {
            out += ", ";
        }
        const auto& elem = arr.at(i);
        if (elem.is_array()) {
            write_toml_inline_array(out, elem.as_array());
        } else if (elem.is_table()) {
            write_toml_inline_table(out, elem.as_table());
        } else {
            write_toml_simple(out, elem);
        }
    }
    out += ']';
}

// Write an inline table: { key = value, ... }
static void write_toml_inline_table(std::string& out, const Table& table) {
    out += '{';
    bool first = true;
    for (const auto& pair : table) {
        if (!first) {
            out += ", ";
        }
        first = false;
        out += escape_toml_key(pair.first);
        out += " = ";
        const auto& val = pair.second;
        if (val.is_table()) {
            write_toml_inline_table(out, val.as_table());
        } else if (val.is_array()) {
            write_toml_inline_array(out, val.as_array());
        } else {
            write_toml_simple(out, val);
        }
    }
    out += '}';
}

// Write a multiline array
static void write_toml_multiline_array(std::string& out, const Array& arr,
                                       const FormatOptions& opts,
                                       size_t indent_level) {
    std::string indent = make_indent(indent_level, opts);
    out += "[\n";
    for (size_t i = 0; i < arr.size(); ++i) {
        out += indent;
        out += make_indent(1, opts);
        const auto& elem = arr.at(i);
        if (elem.is_table()) {
            // For tables in arrays, use inline format
            write_toml_inline_table(out, elem.as_table());
        } else if (elem.is_array()) {
            write_toml_inline_array(out, elem.as_array());
        } else {
            write_toml_simple(out, elem);
        }
        out += ",\n";
    }
    out += indent;
    out += ']';
}

// Write a table section with [key] syntax
static void write_toml_table_section(std::string& out, const Table& table,
                                     const std::string& prefix,
                                     const FormatOptions& opts,
                                     size_t indent_level) {
    std::string indent = make_indent(indent_level, opts);
    auto keys = get_keys(table, opts.sorted_keys);

    // First pass: write simple values
    for (const auto& key : keys) {
        const auto& val = table.at(key);
        if (is_simple_value(val)) {
            out += indent;
            out += escape_toml_key(key);
            out += " = ";
            write_toml_simple(out, val);
            out += '\n';
        }
    }

    // Second pass: write inline arrays (arrays that don't contain tables)
    for (const auto& key : keys) {
        const auto& val = table.at(key);
        if (val.is_array() && !array_contains_tables(val.as_array())) {
            out += indent;
            out += escape_toml_key(key);
            out += " = ";
            write_toml_inline_array(out, val.as_array());
            out += '\n';
        }
    }

    // Third pass: write inline tables (simple nested tables)
    for (const auto& key : keys) {
        const auto& val = table.at(key);
        if (val.is_table() && val.as_table().size() <= 3) {
            // Use inline format for small tables
            out += indent;
            out += escape_toml_key(key);
            out += " = ";
            write_toml_inline_table(out, val.as_table());
            out += '\n';
        }
    }

    // Fourth pass: write complex tables with [section] syntax
    for (const auto& key : keys) {
        const auto& val = table.at(key);
        if (val.is_table() && val.as_table().size() > 3) {
            std::string new_prefix = prefix.empty()
                                         ? escape_toml_key(key)
                                         : prefix + "." + escape_toml_key(key);
            out += "\n[";
            out += new_prefix;
            out += "]\n";
            write_toml_table_section(out, val.as_table(), new_prefix, opts,
                                     indent_level);
        }
    }

    // Fifth pass: write arrays of tables with [[section]] syntax
    for (const auto& key : keys) {
        const auto& val = table.at(key);
        if (val.is_array() && array_contains_tables(val.as_array())) {
            const auto& arr = val.as_array();
            std::string new_prefix = prefix.empty()
                                         ? escape_toml_key(key)
                                         : prefix + "." + escape_toml_key(key);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (arr.at(i).is_table()) {
                    out += "\n[[";
                    out += new_prefix;
                    out += "]]\n";
                    write_toml_table_section(out, arr.at(i).as_table(),
                                             new_prefix, opts, indent_level);
                }
            }
        }
    }
}

// Main recursive writer
static void write_toml_value(std::string& out, const Value& value,
                             const FormatOptions& opts, size_t indent_level) {
    switch (value.type()) {
    case kNull:
        break;
    case kBoolean:
    case kInteger:
    case kUInteger:
    case kFloat:
    case kString:
    case kDateTime:
        write_toml_simple(out, value);
        break;
    case kArray: {
        const auto& arr = value.as_array();
        if (arr.empty()) {
            out += "[]";
        } else if (array_contains_tables(arr)) {
            write_toml_multiline_array(out, arr, opts, indent_level);
        } else {
            write_toml_inline_array(out, arr);
        }
        break;
    }
    case kTable: {
        const auto& table = value.as_table();
        if (table.empty()) {
            out += "{}";
        } else {
            write_toml_table_section(out, table, "", opts, indent_level);
        }
        break;
    }
    }
}

// Public API: formatted TOML output
std::string to_toml(const Value& value, const FormatOptions& opts) {
    if (value.is_null()) {
        return "";
    }
    if (!value.is_table()) {
        // For non-table values, just use simple serialization
        std::string result;
        write_toml_value(result, value, opts, 0);
        return result;
    }
    // For tables, use section-based formatting
    std::string result;
    write_toml_table_section(result, value.as_table(), "", opts, 0);
    // Remove trailing newline if present
    while (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

} // namespace toml
