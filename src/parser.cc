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

#include "src/parser.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace TOML {

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

ParseResult Parser::parse(const std::string& str) {
    return parse(str.data(), str.size());
}

ParseResult Parser::parse(const char* data, size_t len) {
    lexer_ = Lexer(data, len);
    error_ = Error();

    Table root;

    skip_newlines_and_comments();

    while (true) {
        Lexer::Token tok = peek();
        if (tok.type == Lexer::TokenType::kEof)
            break;

        if (tok.type == Lexer::TokenType::kLeftBracket) {
            // Table header: [table] or [[array of tables]]
            advance(); // consume first [

            bool is_array_of_tables = false;
            if (peek().type == Lexer::TokenType::kLeftBracket) {
                is_array_of_tables = true;
                advance(); // consume second [
            }

            std::vector<std::string> keys = parse_key();
            if (!error_.ok())
                return ParseResult{Value(), error_};

            if (is_array_of_tables) {
                expect(Lexer::TokenType::kRightBracket);
                expect(Lexer::TokenType::kRightBracket);
            } else {
                expect(Lexer::TokenType::kRightBracket);
            }
            if (!error_.ok())
                return ParseResult{Value(), error_};

            skip_newlines_and_comments();

            if (is_array_of_tables) {
                // Build the full key path for tracking
                std::string key_path;
                for (size_t i = 0; i < keys.size(); i++) {
                    if (i > 0) key_path += '.';
                    key_path += keys[i];
                }

                // Navigate to the parent, creating intermediate tables.
                Table* parent = navigate_to_subtable(&root, keys, true);
                if (!parent) {
                    if (error_.ok())
                        error("failed to create table for array of tables");
                    return ParseResult{Value(), error_};
                }
                const std::string& last_key = keys.back();

                // Find or create the array
                if (!parent->contains(last_key)) {
                    parent->insert(last_key, Value(Array()));
                    array_of_tables_keys_.insert(key_path);
                } else if (!(*parent)[last_key].is_array()) {
                    error("key '" + last_key + "' is already defined as a non-array");
                    return ParseResult{Value(), error_};
                } else if (array_of_tables_keys_.find(key_path) == array_of_tables_keys_.end()) {
                    // Array exists but was defined inline (e.g., fruits = []), not via [[fruits]]
                    error("key '" + last_key + "' is already defined as an inline array");
                    return ParseResult{Value(), error_};
                }

                // Create a new table and parse into it
                Table new_table;
                parse_key_value_pairs(new_table);
                if (!error_.ok())
                    return ParseResult{Value(), error_};

                // Navigate again to re-acquire parent pointer —
                // parse_key_value_pairs may have modified root via
                // navigate_to_subtable calls that caused rehash.
                parent = navigate_to_subtable(&root, keys, true);
                if (!parent) {
                    if (error_.ok())
                        error("failed to re-acquire parent for array of tables");
                    return ParseResult{Value(), error_};
                }

                Array& arr = (*parent)[last_key].as_array_mut();
                arr.push_back(Value(std::move(new_table)));
            } else {
                // Standard table header: [a.b.c]
                // Build the full key path for duplicate detection
                std::string header_path;
                for (size_t i = 0; i < keys.size(); i++) {
                    if (i > 0) header_path += '.';
                    header_path += keys[i];
                }

                // TOML spec: each table header may only be defined once
                if (defined_table_headers_.count(header_path)) {
                    error("duplicate table header [" + header_path + "]");
                    return ParseResult{Value(), error_};
                }

                // navigate_to_subtable handles all keys except the last.
                // We need to ensure the last key creates/finds a subtable.
                Table* parent = navigate_to_subtable(&root, keys, true);
                if (!parent) {
                    if (error_.ok())
                        error("failed to create table");
                    return ParseResult{Value(), error_};
                }
                const std::string& last_key = keys.back();

                // Re-acquire parent pointer (navigate_to_subtable may have rehashed root)
                if (keys.size() > 1) {
                    parent = navigate_to_subtable(&root, keys, true);
                    if (!parent) {
                        if (error_.ok())
                            error("failed to re-acquire parent for table");
                        return ParseResult{Value(), error_};
                    }
                }

                if (parent->contains(last_key)) {
                    Value& v = (*parent)[last_key];
                    if (v.is_table()) {
                        // TOML spec: tables implicitly created by dotted keys
                        // cannot be redefined by a [table] header.
                        if (v.as_table().is_from_dotted_key()) {
                            error("cannot redefine table [" + header_path +
                                  "] — it was implicitly created by a dotted key");
                            return ParseResult{Value(), error_};
                        }
                        parse_key_value_pairs(v.as_table_mut());
                    } else if (v.is_array() && !v.as_array().empty() && v.as_array().back().is_table()) {
                        parse_key_value_pairs(v.as_array_mut().back().as_table_mut());
                    } else {
                        error("key '" + last_key + "' is already defined as a non-table");
                        return ParseResult{Value(), error_};
                    }
                } else {
                    // Create the subtable and parse into it
                    auto result = parent->insert(last_key, Value(Table()));
                    Table& subtable = result.first->second.as_table_mut();
                    parse_key_value_pairs(subtable);
                }

                // Record this header path (done after parsing so that
                // the implicit-table check above sees prior headers only)
                defined_table_headers_.insert(header_path);

                if (!error_.ok())
                    return ParseResult{Value(), error_};
            }
        } else {
            // Top-level key-value pair
            parse_key_value_pairs(root);
            if (!error_.ok())
                return ParseResult{Value(), error_};
        }
    }

    return ParseResult{Value(std::move(root)), Error()};
}

// ---------------------------------------------------------------------------
// Value parsing
// ---------------------------------------------------------------------------

Value Parser::parse_value() {
    Lexer::Token tok = peek();

    switch (tok.type) {
    case Lexer::TokenType::kString:
        return parse_string();
    case Lexer::TokenType::kInteger:
        return parse_number();
    case Lexer::TokenType::kFloat:
        return parse_number();
    case Lexer::TokenType::kBoolean:
        return parse_boolean();
    case Lexer::TokenType::kDateTime:
        advance();
        return parse_datetime(tok.text);
    case Lexer::TokenType::kLeftBrace:
        return parse_inline_table();
    case Lexer::TokenType::kLeftBracket:
        return parse_array();
    case Lexer::TokenType::kEof:
        return error("unexpected end of file, expected a value");
    default:
        return error_at(tok, "unexpected token, expected a value");
    }
}

// ---------------------------------------------------------------------------
// Inline table: { key = value, ... }
// ---------------------------------------------------------------------------

Value Parser::parse_inline_table() {
    expect(Lexer::TokenType::kLeftBrace);
    if (!error_.ok())
        return Value();

    Table table;

    // Handle empty inline table
    if (peek().type == Lexer::TokenType::kRightBrace) {
        advance();
        return Value(std::move(table));
    }

    while (true) {
        // Parse a key
        std::vector<std::string> keys = parse_key();
        if (!error_.ok())
            return Value();

        expect(Lexer::TokenType::kEquals);
        if (!error_.ok())
            return Value();

        Value val = parse_value();
        if (!error_.ok())
            return Value();

        // Insert using dotted key logic
        insert_dotted_key(table, keys, std::move(val));
        if (!error_.ok())
            return Value();

        if (match(Lexer::TokenType::kComma)) {
            // More pairs to come
            continue;
        }

        break;
    }

    expect(Lexer::TokenType::kRightBrace);
    if (!error_.ok())
        return Value();

    table.set_inline(true);
    return Value(std::move(table));
}

// ---------------------------------------------------------------------------
// Array: [value, value, ...]
// ---------------------------------------------------------------------------

Value Parser::parse_array() {
    expect(Lexer::TokenType::kLeftBracket);
    if (!error_.ok())
        return Value();

    Array arr;

    skip_newlines_and_comments();

    // Handle empty array
    if (peek().type == Lexer::TokenType::kRightBracket) {
        advance();
        return Value(std::move(arr));
    }

    while (true) {
        skip_newlines_and_comments();

        // Allow trailing comma: if next token is ']', break
        if (peek().type == Lexer::TokenType::kRightBracket)
            break;

        Value val = parse_value();
        if (!error_.ok())
            return Value();

        arr.push_back(std::move(val));

        if (match(Lexer::TokenType::kComma)) {
            // More elements, allow newlines before next element
            continue;
        }

        skip_newlines_and_comments();

        if (peek().type == Lexer::TokenType::kRightBracket)
            break;

        return error("expected ',' or ']' in array");
    }

    expect(Lexer::TokenType::kRightBracket);
    if (!error_.ok())
        return Value();

    return Value(std::move(arr));
}

// ---------------------------------------------------------------------------
// String: The Lexer already handles escape decoding
// ---------------------------------------------------------------------------

Value Parser::parse_string() {
    Lexer::Token tok = advance();
    if (tok.type != Lexer::TokenType::kString)
        return error("expected string");
    return Value(std::move(tok.text));
}

// ---------------------------------------------------------------------------
// Number: integer or float
// ---------------------------------------------------------------------------

Value Parser::parse_number() {
    Lexer::Token tok = advance();

    if (tok.type == Lexer::TokenType::kFloat) {
        const std::string& text = tok.text;

        // Handle special float values: inf, nan
        if (text == "inf" || text == "+inf") {
            return Value(std::numeric_limits<double>::infinity());
        }
        if (text == "-inf") {
            return Value(-std::numeric_limits<double>::infinity());
        }
        if (text == "nan" || text == "+nan") {
            return Value(std::numeric_limits<double>::quiet_NaN());
        }
        if (text == "-nan") {
            return Value(-std::numeric_limits<double>::quiet_NaN());
        }

        // Parse regular float (strip underscores for strtod)
        std::string cleaned;
        cleaned.reserve(text.size());
        for (char ch : text) {
            if (ch != '_')
                cleaned += ch;
        }
        char* end = nullptr;
        double val = std::strtod(cleaned.c_str(), &end);
        if (end != cleaned.c_str() + cleaned.size()) {
            return error("invalid float: " + text);
        }
        return Value(val);
    }

    if (tok.type == Lexer::TokenType::kInteger) {
        const std::string& text = tok.text;
        bool negative = false;
        size_t start = 0;

        if (text[0] == '+') {
            start = 1;
        } else if (text[0] == '-') {
            start = 1;
            negative = true;
        }

        uint64_t uval = 0;

        bool is_hex = false;
        bool is_oct_bin = false;
        if (start + 1 < text.size() && text[start] == '0' &&
            (text[start + 1] == 'x' || text[start + 1] == 'X')) {
            // Hex: 0x...
            is_hex = true;
            for (size_t i = start + 2; i < text.size(); i++) {
                char c = text[i];
                if (c == '_')
                    continue;
                uint64_t digit;
                if (c >= '0' && c <= '9')
                    digit = c - '0';
                else if (c >= 'a' && c <= 'f')
                    digit = 10 + (c - 'a');
                else if (c >= 'A' && c <= 'F')
                    digit = 10 + (c - 'A');
                else
                    return error("invalid hex digit in integer: " + text);
                uval = uval * 16 + digit;
            }
        } else if (start + 1 < text.size() && text[start] == '0' &&
                   (text[start + 1] == 'o' || text[start + 1] == 'O')) {
            // Octal: 0o...
            is_oct_bin = true;
            for (size_t i = start + 2; i < text.size(); i++) {
                char c = text[i];
                if (c == '_')
                    continue;
                if (c < '0' || c > '7')
                    return error("invalid octal digit in integer: " + text);
                uval = uval * 8 + (c - '0');
            }
        } else if (start + 1 < text.size() && text[start] == '0' &&
                   (text[start + 1] == 'b' || text[start + 1] == 'B')) {
            // Binary: 0b...
            is_oct_bin = true;
            for (size_t i = start + 2; i < text.size(); i++) {
                char c = text[i];
                if (c == '_')
                    continue;
                if (c != '0' && c != '1')
                    return error("invalid binary digit in integer: " + text);
                uval = uval * 2 + (c - '0');
            }
        } else {
            // Decimal
            for (size_t i = start; i < text.size(); i++) {
                char c = text[i];
                if (c == '_')
                    continue;
                if (c < '0' || c > '9')
                    return error("invalid digit in integer: " + text);
                uval = uval * 10 + (c - '0');
            }
        }

        if (negative) {
            // TOML integers are int64_t: range -9223372036854775808 to 9223372036854775807
            if (uval > static_cast<uint64_t>(INT64_MAX) + 1)
                return error("integer out of range: " + text);
            return Value(static_cast<int64_t>(-static_cast<int64_t>(uval)));
        }

        // Hex values are always stored as unsigned (commonly used for
        // bit masks, addresses, etc. where unsigned is natural).
        if (is_hex)
            return Value(static_cast<uint64_t>(uval));

        if (uval > INT64_MAX)
            return Value(static_cast<uint64_t>(uval));
        return Value(static_cast<int64_t>(uval));
    }

    return error("expected number");
}

// ---------------------------------------------------------------------------
// DateTime
// ---------------------------------------------------------------------------

Value Parser::parse_datetime(const std::string& text) {
    DateTime dt;

    // Helper to parse a fixed number of digits
    auto parse_digits = [&](size_t& pos, int count) -> int {
        int val = 0;
        for (int i = 0; i < count; i++) {
            if (pos >= text.size() || text[pos] < '0' || text[pos] > '9')
                return -1;
            val = val * 10 + (text[pos] - '0');
            pos++;
        }
        return val;
    };

    size_t pos = 0;

    // Determine what kind of datetime this is
    // Full datetime: YYYY-MM-DD...  (starts with 4 digits and a dash)
    // Time only: HH:MM:SS...       (starts with 2 digits and a colon)
    // Date only: YYYY-MM-DD        (starts with 4 digits, dash, 2 digits, dash, 2 digits)

    // Check if it starts with a time (HH:MM:SS pattern)
    bool looks_like_time =
        text.size() >= 5 && std::isdigit(text[0]) && std::isdigit(text[1]) && text[2] == ':' &&
        std::isdigit(text[3]) && std::isdigit(text[4]);

    // Check if it starts with a date (YYYY-MM-DD pattern)
    bool looks_like_date = text.size() >= 10 && std::isdigit(text[0]) && std::isdigit(text[1]) &&
                           std::isdigit(text[2]) && std::isdigit(text[3]) && text[4] == '-' &&
                           std::isdigit(text[5]) && std::isdigit(text[6]) && text[7] == '-' &&
                           std::isdigit(text[8]) && std::isdigit(text[9]);

    if (looks_like_date) {
        // Parse date part: YYYY-MM-DD
        int year = parse_digits(pos, 4);
        pos++; // skip '-'
        int month = parse_digits(pos, 2);
        pos++; // skip '-'
        int day = parse_digits(pos, 2);

        dt.year = static_cast<uint16_t>(year);
        dt.month = static_cast<uint8_t>(month);
        dt.day = static_cast<uint8_t>(day);
        dt.present = DateTime::kYear | DateTime::kMonth | DateTime::kDay;

        // Check for optional time part
        if (pos < text.size() && (text[pos] == 'T' || text[pos] == ' ' || text[pos] == 't')) {
            dt.time_separator = text[pos];
            pos++; // skip T/space

            int hour = parse_digits(pos, 2);
            pos++; // skip ':'
            int minute = parse_digits(pos, 2);
            pos++; // skip ':'
            int second = parse_digits(pos, 2);

            dt.hour = static_cast<uint8_t>(hour);
            dt.minute = static_cast<uint8_t>(minute);
            dt.second = static_cast<uint8_t>(second);
            dt.present |= DateTime::kHour | DateTime::kMinute | DateTime::kSecond;

            // Optional fractional seconds
            if (pos < text.size() && text[pos] == '.') {
                pos++; // skip '.'
                int us = 0;
                int us_digits = 0;
                while (pos < text.size() && std::isdigit(text[pos]) && us_digits < 6) {
                    us = us * 10 + (text[pos] - '0');
                    pos++;
                    us_digits++;
                }
                // Pad to microseconds
                for (int i = us_digits; i < 6; i++)
                    us *= 10;
                dt.microsecond = static_cast<uint32_t>(us);
                dt.microsecond_digits = static_cast<uint8_t>(us_digits);
                dt.present |= DateTime::kMicrosecond;
            }

            // Optional timezone
            if (pos < text.size()) {
                if (text[pos] == 'Z' || text[pos] == 'z') {
                    dt.utc_offset = 0;
                    dt.present |= DateTime::kUtcOffset | DateTime::kSpecific;
                } else if (text[pos] == '+' || text[pos] == '-') {
                    bool negative = (text[pos] == '-');
                    pos++;
                    int tzh = parse_digits(pos, 2);
                    pos++; // skip ':'
                    int tzm = parse_digits(pos, 2);
                    int offset = tzh * 3600 + tzm * 60;
                    dt.utc_offset = negative ? -offset : offset;
                    dt.present |= DateTime::kUtcOffset | DateTime::kSpecific;
                }
            }
        }
    } else if (looks_like_time) {
        // Time only: HH:MM:SS
        int hour = parse_digits(pos, 2);
        pos++; // skip ':'
        int minute = parse_digits(pos, 2);
        pos++; // skip ':'
        int second = parse_digits(pos, 2);

        dt.hour = static_cast<uint8_t>(hour);
        dt.minute = static_cast<uint8_t>(minute);
        dt.second = static_cast<uint8_t>(second);
        dt.present = DateTime::kHour | DateTime::kMinute | DateTime::kSecond;

        // Optional fractional seconds
        if (pos < text.size() && text[pos] == '.') {
            pos++;
            int us = 0;
            int us_digits = 0;
            while (pos < text.size() && std::isdigit(text[pos]) && us_digits < 6) {
                us = us * 10 + (text[pos] - '0');
                pos++;
                us_digits++;
            }
            for (int i = us_digits; i < 6; i++)
                us *= 10;
            dt.microsecond = static_cast<uint32_t>(us);
            dt.microsecond_digits = static_cast<uint8_t>(us_digits);
            dt.present |= DateTime::kMicrosecond;
        }
    } else {
        return error("invalid datetime format: " + text);
    }

    return Value(dt);
}

// ---------------------------------------------------------------------------
// Boolean
// ---------------------------------------------------------------------------

Value Parser::parse_boolean() {
    Lexer::Token tok = advance();
    if (tok.type != Lexer::TokenType::kBoolean)
        return error("expected boolean");
    return Value(tok.text == "true");
}

// ---------------------------------------------------------------------------
// Token helpers
// ---------------------------------------------------------------------------

Lexer::Token Parser::expect(Lexer::TokenType type) {
    Lexer::Token tok = peek();
    if (tok.type != type) {
        // Build a descriptive error message
        const char* expected_name = "token";
        switch (type) {
        case Lexer::TokenType::kLeftBracket:   expected_name = "'['"; break;
        case Lexer::TokenType::kRightBracket:  expected_name = "']'"; break;
        case Lexer::TokenType::kLeftBrace:     expected_name = "'{'"; break;
        case Lexer::TokenType::kRightBrace:    expected_name = "'}'"; break;
        case Lexer::TokenType::kEquals:        expected_name = "'='"; break;
        case Lexer::TokenType::kDot:           expected_name = "'.'"; break;
        case Lexer::TokenType::kComma:         expected_name = "','"; break;
        case Lexer::TokenType::kColon:         expected_name = "':'"; break;
        case Lexer::TokenType::kString:        expected_name = "string"; break;
        case Lexer::TokenType::kInteger:       expected_name = "integer"; break;
        case Lexer::TokenType::kFloat:         expected_name = "float"; break;
        case Lexer::TokenType::kBoolean:       expected_name = "boolean"; break;
        case Lexer::TokenType::kDateTime:      expected_name = "datetime"; break;
        case Lexer::TokenType::kNewline:       expected_name = "newline"; break;
        case Lexer::TokenType::kEof:           expected_name = "end of file"; break;
        case Lexer::TokenType::kComment:       expected_name = "comment"; break;
        case Lexer::TokenType::kError:         expected_name = "error"; break;
        }
        std::string msg = std::string("expected ") + expected_name;
        error_at(tok, msg.c_str());
        return tok;
    }
    return advance();
}

bool Parser::match(Lexer::TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Lexer::Token Parser::advance() {
    return lexer_.next();
}

Lexer::Token Parser::peek() {
    return lexer_.peek();
}

void Parser::skip_newlines_and_comments() {
    while (true) {
        Lexer::Token tok = peek();
        if (tok.type == Lexer::TokenType::kNewline ||
            tok.type == Lexer::TokenType::kComment) {
            advance();
        } else {
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Error helpers
// ---------------------------------------------------------------------------

Value Parser::error(const char* message) {
    SourceLocation loc;
    loc.line = lexer_.current_line();
    loc.column = lexer_.current_column();
    error_.location = loc;
    error_.message = message;
    return Value();
}

Value Parser::error(const std::string& message) {
    return error(message.c_str());
}

Value Parser::error_at(const Lexer::Token& token, const char* message) {
    error_.location.line = token.line;
    error_.location.column = token.column;
    error_.message = message;
    return Value();
}

SourceLocation Parser::current_location() const {
    SourceLocation loc;
    loc.line = lexer_.current_line();
    loc.column = lexer_.current_column();
    return loc;
}

// ---------------------------------------------------------------------------
// Key-value pair parsing
// ---------------------------------------------------------------------------

void Parser::parse_key_value_pairs(Table& table) {
    while (true) {
        skip_newlines_and_comments();

        Lexer::Token tok = peek();

        // Stop at table headers, EOF, or closing braces/brackets
        if (tok.type == Lexer::TokenType::kEof ||
            tok.type == Lexer::TokenType::kLeftBracket ||
            tok.type == Lexer::TokenType::kRightBrace ||
            tok.type == Lexer::TokenType::kRightBracket) {
            break;
        }

        // If we're inside an inline table/array, stop at comma
        // This is handled by the calling function

        // Parse a key
        std::vector<std::string> keys = parse_key();
        if (!error_.ok())
            return;

        expect(Lexer::TokenType::kEquals);
        if (!error_.ok())
            return;

        Value val = parse_value();
        if (!error_.ok())
            return;

        // Insert the value using dotted key logic
        insert_dotted_key(table, keys, std::move(val));
        if (!error_.ok())
            return;

        // TOML requires newline/comment/EOF/closing-bracket after a key-value
        // pair.  Check the very next token *before* skipping newlines, so that
        // a bare key on the same line is correctly rejected.
        {
            Lexer::Token next = peek();
            if (next.type != Lexer::TokenType::kNewline &&
                next.type != Lexer::TokenType::kComment &&
                next.type != Lexer::TokenType::kEof &&
                next.type != Lexer::TokenType::kLeftBracket &&
                next.type != Lexer::TokenType::kRightBrace &&
                next.type != Lexer::TokenType::kRightBracket) {
                error_at(next, "expected newline or comment between key-value pairs");
                return;
            }
        }

        // Now consume the newline/comment
        skip_newlines_and_comments();
    }
}

// ---------------------------------------------------------------------------
// Key parsing
// ---------------------------------------------------------------------------

std::vector<std::string> Parser::parse_key() {
    std::vector<std::string> keys;

    // Parse first key part
    Lexer::Token tok = peek();
    if (tok.type == Lexer::TokenType::kString) {
        advance();
        keys.push_back(std::move(tok.text));
    } else if (tok.type == Lexer::TokenType::kBoolean) {
        // Bare key: true or false are valid bare keys in TOML
        advance();
        keys.push_back(std::move(tok.text));
    } else if (tok.type == Lexer::TokenType::kInteger) {
        // Bare key: numeric bare keys
        advance();
        keys.push_back(std::move(tok.text));
    } else if (tok.type == Lexer::TokenType::kFloat) {
        // Bare key: inf, nan are valid bare keys.
        // A float like "3.14159" in key context is a dotted key: 3 and 14159.
        advance();
        {
            const std::string& text = tok.text;
            size_t dot = text.find('.');
            if (dot != std::string::npos) {
                // Split into parts around '.'
                keys.push_back(text.substr(0, dot));
                keys.push_back(text.substr(dot + 1));
            } else {
                keys.push_back(text);
            }
        }
    } else if (tok.type == Lexer::TokenType::kDateTime) {
        // Bare key: datetime-like text can be a bare key
        advance();
        keys.push_back(std::move(tok.text));
    } else if (tok.type == Lexer::TokenType::kDot) {
        error_keys("unexpected '.' at start of key");
        return keys;
    } else if (tok.type == Lexer::TokenType::kEquals) {
        error_keys("empty key");
        return keys;
    } else if (tok.type == Lexer::TokenType::kLeftBracket ||
               tok.type == Lexer::TokenType::kRightBracket) {
        error_keys("unexpected bracket in key");
        return keys;
    } else if (tok.type == Lexer::TokenType::kEof) {
        error_keys("unexpected end of file in key");
        return keys;
    } else if (tok.type == Lexer::TokenType::kError) {
        error_at(tok, tok.text.c_str());
        return keys;
    } else {
        // Other punctuation: treat the token text as a bare key
        // This handles simple bare keys like: key = "value"
        advance();
        keys.push_back(std::move(tok.text));
    }

    // Parse additional key parts separated by '.'
    while (peek().type == Lexer::TokenType::kDot) {
        advance(); // consume '.'

        tok = peek();
        if (tok.type == Lexer::TokenType::kString) {
            advance();
            keys.push_back(std::move(tok.text));
        } else if (tok.type == Lexer::TokenType::kBoolean) {
            advance();
            keys.push_back(std::move(tok.text));
        } else if (tok.type == Lexer::TokenType::kInteger) {
            advance();
            keys.push_back(std::move(tok.text));
        } else if (tok.type == Lexer::TokenType::kFloat) {
            advance();
            {
                const std::string& text = tok.text;
                size_t dot = text.find('.');
                if (dot != std::string::npos) {
                    keys.push_back(text.substr(0, dot));
                    keys.push_back(text.substr(dot + 1));
                } else {
                    keys.push_back(text);
                }
            }
        } else if (tok.type == Lexer::TokenType::kDateTime) {
            advance();
            keys.push_back(std::move(tok.text));
        } else if (tok.type == Lexer::TokenType::kDot) {
            error("unexpected '.' in key");
            return keys;
        } else if (tok.type == Lexer::TokenType::kEquals) {
            error("unexpected '=' in key");
            return keys;
        } else if (tok.type == Lexer::TokenType::kEof) {
            error("unexpected end of file in key");
            return keys;
        } else if (tok.type == Lexer::TokenType::kError) {
            error_at(tok, tok.text.c_str());
            return keys;
        } else {
            advance();
            keys.push_back(std::move(tok.text));
        }
    }

    return keys;
}

// ---------------------------------------------------------------------------
// Table navigation helpers
// ---------------------------------------------------------------------------

Table* Parser::navigate_to_subtable(Table* root, const std::vector<std::string>& keys, bool create,
                                    bool mark_as_dotted_key) {
    Table* current = root;
    // Navigate through all keys except the last one
    for (size_t i = 0; i + 1 < keys.size(); i++) {
        const std::string& key = keys[i];
        if (current->contains(key)) {
            Value& v = (*current)[key];
            if (v.is_table()) {
                // TOML spec: inline tables cannot be extended after definition
                if (v.as_table().is_inline()) {
                    error("cannot extend inline table '" + key + "'");
                    return nullptr;
                }
                current = &v.as_table_mut();
            } else if (v.is_array() && !v.as_array().empty() && v.as_array().back().is_table()) {
                // TOML spec: [[fruits]] followed by [fruits.physical] navigates
                // into the last element of the array of tables.
                current = &v.as_array_mut().back().as_table_mut();
            } else {
                error("key '" + key + "' is already defined as a non-table");
                return nullptr;
            }
        } else if (create) {
            auto result = current->insert(key, Value(Table()));
            current = &result.first->second.as_table_mut();
            if (mark_as_dotted_key)
                current->set_from_dotted_key(true);
        } else {
            return nullptr;
        }
    }
    return current;
}

void Parser::insert_dotted_key(Table& table, const std::vector<std::string>& keys, Value value) {
    if (keys.size() == 1) {
        // Simple key
        const std::string& key = keys[0];
        if (table.contains(key)) {
            error("duplicate key '" + key + "'");
            return;
        }
        table.insert(key, std::move(value));
    } else {
        // Dotted key: navigate to the parent table.
        // Mark any intermediate tables created here as from-dotted-key so
        // that later [table] headers cannot redefine them.
        Table* parent = navigate_to_subtable(&table, keys, true, /*mark_as_dotted_key=*/true);
        if (!parent) {
            if (error_.ok())
                error("failed to navigate dotted key path");
            return;
        }
        const std::string& last_key = keys.back();
        if (parent->contains(last_key)) {
            error("duplicate key '" + last_key + "'");
            return;
        }
        parent->insert(last_key, std::move(value));
    }
}

// Helper to set error from parse_key (which returns vector, not Value)
// This is a bit awkward but necessary since parse_key returns vector<string>
// We use a helper that sets error_ directly
void Parser::error_keys(const char* message) {
    SourceLocation loc;
    loc.line = lexer_.current_line();
    loc.column = lexer_.current_column();
    error_.location = loc;
    error_.message = message;
}

// ---------------------------------------------------------------------------
// Free functions (declared in toml/reader.h)
// ---------------------------------------------------------------------------

ParseResult parse_string(const char* data, size_t len) {
    Parser parser;
    return parser.parse(data, len);
}

ParseResult parse_string(const std::string& str) {
    Parser parser;
    return parser.parse(str);
}

ParseResult parse_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Error err;
        err.location = {0, 0};
        err.message = "failed to open file: " + path;
        return ParseResult{Value(), err};
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    Parser parser;
    return parser.parse(content.data(), content.size());
}

} // namespace TOML
