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

#ifndef TOML_LEXER_H_
#define TOML_LEXER_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace toml {

class Lexer {
public:
    enum class TokenType : uint8_t {
        // literals
        kString,    // "quoted string" or 'literal string'
        kInteger,   // 123, -456, 0xDEAD, 0o755, 0b1101
        kFloat,     // 3.14, -1.0, 5e10, inf, nan
        kBoolean,   // true, false
        kDateTime,  // 2023-01-15, 12:30:45, etc.

        // punctuation
        kLeftBracket,   // [
        kRightBracket,  // ]
        kLeftBrace,     // {
        kRightBrace,    // }
        kEquals,        // =
        kDot,           // .
        kComma,         // ,
        kColon,         // :

        // special
        kNewline,   // \n or \r\n
        kComment,   // # to end of line
        kEof,       // end of file
        kError,     // lexical error
    };

    struct Token {
        TokenType type;
        std::string text;
        size_t line;
        size_t column;
    };

    Lexer();
    Lexer(const char* data, size_t len);

    Token next();
    Token peek();
    void skip_whitespace_and_comments();

    size_t current_line() const { return line_; }
    size_t current_column() const { return column_; }

private:
    // character helpers
    char peek_char() const;
    char peek_char_at(size_t offset) const;
    char advance();
    bool match(char expected);
    bool is_at_end() const;

    // token construction
    Token make_token(TokenType type, size_t start_line, size_t start_col);
    Token error_token(const char* message, size_t start_line, size_t start_col);

    // string lexing
    Token lex_string();
    Token lex_basic_string(size_t start_line, size_t start_col);
    Token lex_literal_string(size_t start_line, size_t start_col);
    Token lex_multi_line_basic_string(size_t start_line, size_t start_col);
    Token lex_multi_line_literal_string(size_t start_line, size_t start_col);

    // number lexing
    Token lex_number(size_t start_line, size_t start_col);
    Token lex_hex_integer(size_t start_line, size_t start_col, size_t raw_start);
    Token lex_oct_integer(size_t start_line, size_t start_col, size_t raw_start);
    Token lex_bin_integer(size_t start_line, size_t start_col, size_t raw_start);

    // other lexing
    Token lex_datetime(size_t start_line, size_t start_col);
    Token lex_comment(size_t start_line, size_t start_col);

    // classification helpers
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_hex_digit(char c) const;
    bool is_oct_digit(char c) const;
    bool is_bin_digit(char c) const;
    bool is_whitespace(char c) const;

    const char* data_;
    size_t len_;
    size_t pos_;
    size_t line_;
    size_t column_;

    Token peeked_token_;
    bool has_peeked_;
};

} // namespace toml

#endif // TOML_LEXER_H_
