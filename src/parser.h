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

#ifndef TOML_PARSER_H_
#define TOML_PARSER_H_

#include "toml/value.h"
#include "toml/error.h"
#include "src/lexer.h"
#include <set>

namespace toml {

class Parser {
public:
    ParseResult parse(const char* data, size_t len);
    ParseResult parse(const std::string& str);

private:
    // Core parsing methods
    Value parse_value();
    Value parse_inline_table();
    Value parse_array();
    Value parse_string();
    Value parse_number();
    Value parse_datetime(const std::string& text);
    Value parse_boolean();

    // Helper methods
    Lexer::Token expect(Lexer::TokenType type);
    bool match(Lexer::TokenType type);
    Lexer::Token advance();
    Lexer::Token peek();
    void skip_newlines_and_comments();

    // Error handling
    Value error(const char* message);
    Value error(const std::string& message);
    Value error_at(const Lexer::Token& token, const char* message);
    SourceLocation current_location() const;

    // Table construction helpers
    void parse_key_value_pairs(Table& table);
    std::vector<std::string> parse_key();
    Table* navigate_to_subtable(Table* root, const std::vector<std::string>& keys, bool create,
                                bool mark_as_dotted_key = false);
    void insert_dotted_key(Table& table, const std::vector<std::string>& keys, Value value);
    void error_keys(const char* message);

    Lexer lexer_;
    Error error_;
    // Tracks keys defined via [[key]] syntax (array of tables).
    // Used to reject conflicts like: fruits = []  followed by  [[fruits]]
    std::set<std::string> array_of_tables_keys_;
    // Tracks table paths defined via [key] syntax (standard table headers).
    // Used to reject duplicate table header definitions.
    std::set<std::string> defined_table_headers_;
};

} // namespace toml

#endif // TOML_PARSER_H_
