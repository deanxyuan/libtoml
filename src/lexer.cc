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

#include "src/lexer.h"

namespace TOML {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Lexer::Lexer()
    : data_(nullptr)
    , len_(0)
    , pos_(0)
    , line_(1)
    , column_(1)
    , has_peeked_(false) {}

Lexer::Lexer(const char* data, size_t len)
    : data_(data)
    , len_(len)
    , pos_(0)
    , line_(1)
    , column_(1)
    , has_peeked_(false) {}

// ---------------------------------------------------------------------------
// Character helpers
// ---------------------------------------------------------------------------

char Lexer::peek_char() const {
    if (is_at_end())
        return '\0';
    return data_[pos_];
}

char Lexer::peek_char_at(size_t offset) const {
    size_t idx = pos_ + offset;
    if (idx >= len_)
        return '\0';
    return data_[idx];
}

char Lexer::advance() {
    if (is_at_end())
        return '\0';
    char c = data_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else if (c == '\r') {
        if (pos_ < len_ && data_[pos_] == '\n')
            pos_++;
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (is_at_end())
        return false;
    if (data_[pos_] != expected)
        return false;
    advance();
    return true;
}

bool Lexer::is_at_end() const {
    return pos_ >= len_;
}

// ---------------------------------------------------------------------------
// Classification helpers
// ---------------------------------------------------------------------------

bool Lexer::is_alpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::is_digit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::is_hex_digit(char c) const {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexer::is_oct_digit(char c) const {
    return c >= '0' && c <= '7';
}

bool Lexer::is_bin_digit(char c) const {
    return c == '0' || c == '1';
}

bool Lexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t';
}

// ---------------------------------------------------------------------------
// Token construction helpers
// ---------------------------------------------------------------------------

Lexer::Token Lexer::make_token(TokenType type, size_t start_line, size_t start_col) {
    Token tok;
    tok.type   = type;
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

Lexer::Token Lexer::error_token(const char* message, size_t start_line, size_t start_col) {
    Token tok;
    tok.type   = TokenType::kError;
    tok.text   = message;
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

// ---------------------------------------------------------------------------
// Unicode escape helper
// ---------------------------------------------------------------------------

// Decode a \uXXXX (4 hex digits) or \UXXXXXXXX (8 hex digits) escape.
// Appends the UTF-8 encoding to `result`. Returns true on success.
static bool decode_unicode_escape(const char* data, size_t len, size_t& pos,
                                  int ndigits, std::string& result) {
    uint32_t codepoint = 0;
    for (int i = 0; i < ndigits; i++) {
        if (pos >= len)
            return false;
        char h = data[pos++];
        if (h >= '0' && h <= '9')
            codepoint = (codepoint << 4) + (h - '0');
        else if (h >= 'a' && h <= 'f')
            codepoint = (codepoint << 4) + 10 + (h - 'a');
        else if (h >= 'A' && h <= 'F')
            codepoint = (codepoint << 4) + 10 + (h - 'A');
        else
            return false;
    }
    // encode as UTF-8
    if (codepoint <= 0x7F) {
        result += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        result += static_cast<char>(0xF0 | (codepoint >> 18));
        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

Lexer::Token Lexer::next() {
    if (has_peeked_) {
        has_peeked_ = false;
        return peeked_token_;
    }

    // skip whitespace (spaces and tabs) -- they are not meaningful tokens
    while (!is_at_end() && is_whitespace(peek_char())) {
        advance();
    }

    if (is_at_end()) {
        Token tok;
        tok.type   = TokenType::kEof;
        tok.line   = line_;
        tok.column = column_;
        return tok;
    }

    size_t start_line = line_;
    size_t start_col  = column_;
    char c            = peek_char();

    // newline
    if (c == '\n' || c == '\r') {
        advance();
        Token tok;
        tok.type   = TokenType::kNewline;
        tok.text   = "\n";
        tok.line   = start_line;
        tok.column = start_col;
        return tok;
    }

    // comment
    if (c == '#') {
        return lex_comment(start_line, start_col);
    }

    // punctuation
    switch (c) {
    case '[':
        advance();
        return Token{TokenType::kLeftBracket, "[", start_line, start_col};
    case ']':
        advance();
        return Token{TokenType::kRightBracket, "]", start_line, start_col};
    case '{':
        advance();
        return Token{TokenType::kLeftBrace, "{", start_line, start_col};
    case '}':
        advance();
        return Token{TokenType::kRightBrace, "}", start_line, start_col};
    case '=':
        advance();
        return Token{TokenType::kEquals, "=", start_line, start_col};
    case '.':
        advance();
        return Token{TokenType::kDot, ".", start_line, start_col};
    case ',':
        advance();
        return Token{TokenType::kComma, ",", start_line, start_col};
    case ':':
        advance();
        return Token{TokenType::kColon, ":", start_line, start_col};
    default:
        break;
    }

    // string
    if (c == '"' || c == '\'') {
        return lex_string();
    }

    // number (possibly datetime)
    if (is_digit(c) || c == '+' || c == '-') {
        return lex_number(start_line, start_col);
    }

    // boolean: true / false
    if (c == 't' || c == 'f') {
        const char* kw     = (c == 't') ? "true" : "false";
        size_t      kw_len = (c == 't') ? 4 : 5;
        if (pos_ + kw_len <= len_) {
            bool m = true;
            for (size_t i = 0; i < kw_len; i++) {
                if (data_[pos_ + i] != kw[i]) {
                    m = false;
                    break;
                }
            }
            if (m && (pos_ + kw_len >= len_ ||
                      (!is_alpha(data_[pos_ + kw_len]) && !is_digit(data_[pos_ + kw_len])))) {
                for (size_t i = 0; i < kw_len; i++)
                    advance();
                return Token{TokenType::kBoolean, std::string(kw, kw_len), start_line, start_col};
            }
        }
    }

    // special floats: inf, nan
    if (c == 'i' || c == 'n') {
        const char* kw     = (c == 'i') ? "inf" : "nan";
        size_t      kw_len = 3;
        if (pos_ + kw_len <= len_) {
            bool m = true;
            for (size_t i = 0; i < kw_len; i++) {
                if (data_[pos_ + i] != kw[i]) {
                    m = false;
                    break;
                }
            }
            if (m && (pos_ + kw_len >= len_ ||
                      (!is_alpha(data_[pos_ + kw_len]) && !is_digit(data_[pos_ + kw_len])))) {
                for (size_t i = 0; i < kw_len; i++)
                    advance();
                return Token{TokenType::kFloat, std::string(kw, kw_len), start_line, start_col};
            }
        }
    }

    // bare key: [A-Za-z0-9-_]+
    if (is_alpha(c) || is_digit(c)) {
        size_t start = pos_;
        while (!is_at_end() && (is_alpha(peek_char()) || is_digit(peek_char()) ||
                                 peek_char() == '-' || peek_char() == '_')) {
            advance();
        }
        std::string text(data_ + start, pos_ - start);

        // Check if it's a boolean keyword
        if (text == "true" || text == "false") {
            return Token{TokenType::kBoolean, text, start_line, start_col};
        }
        // Check if it's a special float keyword
        if (text == "inf" || text == "nan") {
            return Token{TokenType::kFloat, text, start_line, start_col};
        }
        // Otherwise it's a bare key (treated as a string token)
        return Token{TokenType::kString, text, start_line, start_col};
    }

    // unrecognized character
    advance();
    std::string msg = "unexpected character '";
    msg += c;
    msg += "'";
    return error_token(msg.c_str(), start_line, start_col);
}

Lexer::Token Lexer::peek() {
    if (!has_peeked_) {
        peeked_token_ = next();
        has_peeked_   = true;
    }
    return peeked_token_;
}

void Lexer::skip_whitespace_and_comments() {
    while (true) {
        if (is_at_end())
            return;
        char c = peek_char();
        if (is_whitespace(c)) {
            advance();
            continue;
        }
        if (c == '#') {
            while (!is_at_end() && peek_char() != '\n' && peek_char() != '\r')
                advance();
            continue;
        }
        return;
    }
}

// ---------------------------------------------------------------------------
// Comment
// ---------------------------------------------------------------------------

Lexer::Token Lexer::lex_comment(size_t start_line, size_t start_col) {
    advance(); // consume '#'
    size_t text_start = pos_;
    while (!is_at_end()) {
        char c = peek_char();
        if (c == '\n' || c == '\r')
            break;
        advance();
    }
    Token tok;
    tok.type   = TokenType::kComment;
    tok.text   = std::string(data_ + text_start, pos_ - text_start);
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

// ---------------------------------------------------------------------------
// String lexing
// ---------------------------------------------------------------------------

Lexer::Token Lexer::lex_string() {
    size_t start_line = line_;
    size_t start_col  = column_;
    char c            = peek_char();

    // multi-line: """ or '''
    if (pos_ + 2 < len_ && data_[pos_ + 1] == c && data_[pos_ + 2] == c) {
        if (c == '"')
            return lex_multi_line_basic_string(start_line, start_col);
        else
            return lex_multi_line_literal_string(start_line, start_col);
    }

    if (c == '"')
        return lex_basic_string(start_line, start_col);
    else
        return lex_literal_string(start_line, start_col);
}

// --- basic string "..." ---

Lexer::Token Lexer::lex_basic_string(size_t start_line, size_t start_col) {
    advance(); // consume opening "

    std::string result;
    while (!is_at_end()) {
        char c = peek_char();
        if (c == '"') {
            advance();
            return Token{TokenType::kString, std::move(result), start_line, start_col};
        }
        if (c == '\\') {
            advance(); // consume backslash
            if (is_at_end())
                return error_token("unterminated escape sequence", start_line, start_col);
            char esc = advance();
            switch (esc) {
            case 'b':
                result += '\b';
                break;
            case 't':
                result += '\t';
                break;
            case 'n':
                result += '\n';
                break;
            case 'f':
                result += '\f';
                break;
            case 'r':
                result += '\r';
                break;
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case 'u': {
                if (!decode_unicode_escape(data_, len_, pos_, 4, result))
                    return error_token("invalid unicode escape", start_line, start_col);
                // update column tracking (advance() doesn't handle raw pos_ increments)
                column_ += 4;
                break;
            }
            case 'U': {
                if (!decode_unicode_escape(data_, len_, pos_, 8, result))
                    return error_token("invalid unicode escape", start_line, start_col);
                column_ += 8;
                break;
            }
            default:
                return error_token("invalid escape character", start_line, start_col);
            }
        } else if (c == '\n' || c == '\r') {
            return error_token("basic string cannot contain newline", start_line, start_col);
        } else {
            result += advance();
        }
    }
    return error_token("unterminated basic string", start_line, start_col);
}

// --- literal string '...' ---

Lexer::Token Lexer::lex_literal_string(size_t start_line, size_t start_col) {
    advance(); // consume opening '

    size_t content_start = pos_;
    while (!is_at_end()) {
        char c = peek_char();
        if (c == '\'') {
            size_t content_end = pos_;
            advance(); // consume closing '
            std::string result(data_ + content_start, content_end - content_start);
            return Token{TokenType::kString, std::move(result), start_line, start_col};
        }
        if (c == '\n' || c == '\r') {
            return error_token("literal string cannot contain newline", start_line, start_col);
        }
        advance();
    }
    return error_token("unterminated literal string", start_line, start_col);
}

// --- multi-line basic string """...""" ---

Lexer::Token Lexer::lex_multi_line_basic_string(size_t start_line, size_t start_col) {
    advance();
    advance();
    advance(); // consume opening """

    // trim the first newline after """
    if (!is_at_end()) {
        char c = peek_char();
        if (c == '\n')
            advance();
        else if (c == '\r')
            advance(); // advance() handles \r\n
    }

    std::string result;
    while (!is_at_end()) {
        char c = peek_char();

        if (c == '"') {
            // count consecutive quotes
            size_t quote_count = 0;
            size_t check       = pos_;
            while (check < len_ && data_[check] == '"') {
                quote_count++;
                check++;
            }
            if (quote_count >= 3) {
                advance();
                advance();
                advance(); // consume closing """
                size_t extra = quote_count - 3;
                for (size_t i = 0; i < extra; i++)
                    result += advance(); // consume extra quotes from input
                return Token{TokenType::kString, std::move(result), start_line, start_col};
            }
            result += advance();
        } else if (c == '\\') {
            // line ending trimming: \<newline> + all whitespace (including newlines)
            if (pos_ + 1 < len_) {
                char nc = data_[pos_ + 1];
                if (nc == '\n' || nc == '\r') {
                    advance(); // consume backslash
                    advance(); // consume newline
                    while (!is_at_end() && (is_whitespace(peek_char()) ||
                                            peek_char() == '\n' || peek_char() == '\r'))
                        advance();
                    continue;
                }
            }

            advance(); // consume backslash
            if (is_at_end())
                return error_token("unterminated escape sequence", start_line, start_col);
            char esc = advance();
            switch (esc) {
            case 'b':
                result += '\b';
                break;
            case 't':
                result += '\t';
                break;
            case 'n':
                result += '\n';
                break;
            case 'f':
                result += '\f';
                break;
            case 'r':
                result += '\r';
                break;
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case 'u': {
                if (!decode_unicode_escape(data_, len_, pos_, 4, result))
                    return error_token("invalid unicode escape", start_line, start_col);
                column_ += 4;
                break;
            }
            case 'U': {
                if (!decode_unicode_escape(data_, len_, pos_, 8, result))
                    return error_token("invalid unicode escape", start_line, start_col);
                column_ += 8;
                break;
            }
            default:
                return error_token("invalid escape character", start_line, start_col);
            }
        } else {
            result += advance();
        }
    }
    return error_token("unterminated multi-line basic string", start_line, start_col);
}

// --- multi-line literal string '''...''' ---

Lexer::Token Lexer::lex_multi_line_literal_string(size_t start_line, size_t start_col) {
    advance();
    advance();
    advance(); // consume opening '''

    // trim the first newline after '''
    if (!is_at_end()) {
        char c = peek_char();
        if (c == '\n')
            advance();
        else if (c == '\r')
            advance();
    }

    size_t content_start = pos_;
    while (!is_at_end()) {
        char c = peek_char();
        if (c == '\'') {
            size_t quote_count = 0;
            size_t check       = pos_;
            while (check < len_ && data_[check] == '\'') {
                quote_count++;
                check++;
            }
            if (quote_count >= 3) {
                size_t content_end = pos_;
                std::string result(data_ + content_start, content_end - content_start);
                advance();
                advance();
                advance(); // consume closing '''
                size_t extra = quote_count - 3;
                for (size_t i = 0; i < extra; i++) {
                    result += advance(); // consume extra quotes from input
                }
                return Token{TokenType::kString, std::move(result), start_line, start_col};
            }
            advance();
        } else {
            advance();
        }
    }
    return error_token("unterminated multi-line literal string", start_line, start_col);
}

// ---------------------------------------------------------------------------
// Number and datetime lexing
// ---------------------------------------------------------------------------

// Try to read a datetime starting from the current position.
// On entry, pos_ points right after the initial 4-digit year and the '-'.
// The full date pattern is: YYYY-MM-DD, possibly followed by time components.
// Returns true if a datetime was successfully consumed, false otherwise.
// On success, `result` contains the full datetime text.
static bool try_lex_datetime(const char* data, size_t len, size_t& pos,
                             size_t& /*line*/, size_t& col, std::string& result) {
    // On entry, pos points right after 'YYYY-'. We expect DD next.
    auto is_d = [](char c) { return c >= '0' && c <= '9'; };

    if (pos + 2 > len)
        return false;
    if (!is_d(data[pos]) || !is_d(data[pos + 1]))
        return false;

    size_t p = pos + 2; // after MM

    // Expect '-' separator between month and day
    if (p >= len || data[p] != '-')
        return false;
    p++; // skip '-'

    // Day digits
    if (p + 2 > len || !is_d(data[p]) || !is_d(data[p+1]))
        return false;
    p += 2; // after DD

    // Check for optional time part: T or space, then HH:MM:SS
    if (p < len && (data[p] == 'T' || data[p] == ' ' || data[p] == 't')) {
        p++; // skip T/space
        // HH:MM:SS
        if (p + 8 <= len && is_d(data[p]) && is_d(data[p + 1]) && data[p + 2] == ':' &&
            is_d(data[p + 3]) && is_d(data[p + 4]) && data[p + 5] == ':' &&
            is_d(data[p + 6]) && is_d(data[p + 7])) {
            p += 8;

            // optional fractional seconds .FFFFFF
            if (p < len && data[p] == '.') {
                p++;
                while (p < len && is_d(data[p]))
                    p++;
            }

            // optional timezone: Z, +HH:MM, -HH:MM
            if (p < len && (data[p] == 'Z' || data[p] == 'z')) {
                p++;
            } else if (p < len && (data[p] == '+' || data[p] == '-')) {
                p++;
                if (p + 5 <= len && is_d(data[p]) && is_d(data[p + 1]) && data[p + 2] == ':' &&
                    is_d(data[p + 3]) && is_d(data[p + 4])) {
                    p += 5;
                } else {
                    return false;
                }
            }
        } else {
            return false; // T/space not followed by valid time
        }
    }

    // Success. Build the result string. The full token starts from before the year.
    // We need to go back: the year (4 chars) + dash (1 char) were consumed before us.
    size_t token_start = pos - 5; // YYYY-
    result.assign(data + token_start, p - token_start);

    // Update pos and column. We don't use advance() because we've already
    // scanned ahead. We just update position tracking.
    size_t consumed = p - pos;
    pos = p;
    col += consumed;
    return true;
}

Lexer::Token Lexer::lex_number(size_t start_line, size_t start_col) {
    // Record the start position for the raw text (includes optional sign).
    size_t raw_start = pos_;

    // consume optional sign
    if (peek_char() == '+' || peek_char() == '-') {
        advance();
    }

    // Check for signed inf/nan: +inf, -inf, +nan, -nan
    {
        char c = peek_char();
        if (c == 'i' || c == 'n') {
            const char* kw     = (c == 'i') ? "inf" : "nan";
            size_t      kw_len = 3;
            if (pos_ + kw_len <= len_) {
                bool m = true;
                for (size_t i = 0; i < kw_len; i++) {
                    if (data_[pos_ + i] != kw[i]) {
                        m = false;
                        break;
                    }
                }
                if (m && (pos_ + kw_len >= len_ ||
                          (!is_alpha(data_[pos_ + kw_len]) && !is_digit(data_[pos_ + kw_len])))) {
                    for (size_t i = 0; i < kw_len; i++)
                        advance();
                    std::string text(data_ + raw_start, pos_ - raw_start);
                    return Token{TokenType::kFloat, std::move(text), start_line, start_col};
                }
            }
        }
    }

    // check for hex/oct/bin prefix: 0x, 0o, 0b
    if (!is_at_end() && peek_char() == '0' && pos_ + 1 < len_) {
        char nc = data_[pos_ + 1];
        if (nc == 'x' || nc == 'X')
            return lex_hex_integer(start_line, start_col, raw_start);
        if (nc == 'o' || nc == 'O')
            return lex_oct_integer(start_line, start_col, raw_start);
        if (nc == 'b' || nc == 'B')
            return lex_bin_integer(start_line, start_col, raw_start);
    }

    // Read integer digits (possibly with underscores).
    bool has_digits = false;
    while (!is_at_end() && is_digit(peek_char())) {
        advance();
        has_digits = true;
    }
    while (!is_at_end() && peek_char() == '_') {
        if (pos_ + 1 < len_ && is_digit(data_[pos_ + 1])) {
            advance(); // _
            advance(); // digit
            has_digits = true;
            while (!is_at_end() && is_digit(peek_char()))
                advance();
        } else {
            break;
        }
    }

    if (!has_digits) {
        return error_token("expected digits in number", start_line, start_col);
    }

    // Check for datetime pattern: YYYY-MM-DD...
    // If we've read exactly 4 digits (no underscores) and the next char is '-',
    // try to parse as a datetime.
    if (peek_char() == '-') {
        // Count how many digits we read (excluding underscores and optional sign).
        size_t digit_count = 0;
        size_t digit_start = raw_start;
        if (digit_start < pos_ && (data_[digit_start] == '+' || data_[digit_start] == '-'))
            digit_start++;
        for (size_t i = digit_start; i < pos_; i++) {
            if (is_digit(data_[i]))
                digit_count++;
        }
        if (digit_count == 4) {
            // Save state before consuming '-'. If datetime parsing fails, we
            // restore to this point and emit the integer (without the '-').
            size_t saved_pos  = pos_;
            size_t saved_line = line_;
            size_t saved_col  = column_;
            advance(); // consume '-'
            std::string dt_result;
            if (try_lex_datetime(data_, len_, pos_, line_, column_, dt_result)) {
                return Token{TokenType::kDateTime, std::move(dt_result), start_line, start_col};
            }
            // Not a valid datetime -- restore to before the '-' and emit the integer.
            pos_    = saved_pos;
            line_   = saved_line;
            column_ = saved_col;
            Token tok;
            tok.type   = TokenType::kInteger;
            tok.text   = std::string(data_ + raw_start, pos_ - raw_start);
            tok.line   = start_line;
            tok.column = start_col;
            return tok;
        }
    }

    // Check for time pattern: HH:MM:SS (exactly 2 digits followed by ':')
    if (peek_char() == ':') {
        // Count digits read (excluding sign)
        size_t digit_start = raw_start;
        if (digit_start < pos_ && (data_[digit_start] == '+' || data_[digit_start] == '-'))
            digit_start++;
        size_t digit_count = 0;
        for (size_t i = digit_start; i < pos_; i++) {
            if (is_digit(data_[i]))
                digit_count++;
        }
        if (digit_count == 2) {
            // Try to parse HH:MM:SS
            size_t saved_pos  = pos_;
            size_t saved_line = line_;
            size_t saved_col  = column_;

            size_t p = pos_; // at ':'
            p++; // skip ':'
            // MM
            if (p + 2 <= len_ && is_digit(data_[p]) && is_digit(data_[p + 1])) {
                p += 2;
                if (p < len_ && data_[p] == ':') {
                    p++; // skip ':'
                    // SS
                    if (p + 2 <= len_ && is_digit(data_[p]) && is_digit(data_[p + 1])) {
                        p += 2;
                        // optional fractional seconds
                        if (p < len_ && data_[p] == '.') {
                            p++;
                            while (p < len_ && is_digit(data_[p]))
                                p++;
                        }
                        // Success — emit the full time token
                        std::string text(data_ + raw_start, p - raw_start);
                        pos_ = p;
                        column_ += (p - saved_pos);
                        return Token{TokenType::kDateTime, std::move(text), start_line, start_col};
                    }
                }
            }
            // Failed — restore
            pos_    = saved_pos;
            line_   = saved_line;
            column_ = saved_col;
        }
    }

    // Not a datetime, continue checking for float patterns.
    bool is_float = false;

    // fractional part: .DIGITS
    if (!is_at_end() && peek_char() == '.' && pos_ + 1 < len_ && is_digit(data_[pos_ + 1])) {
        is_float = true;
        advance(); // .
        while (!is_at_end() && is_digit(peek_char()))
            advance();
        while (!is_at_end() && peek_char() == '_') {
            if (pos_ + 1 < len_ && is_digit(data_[pos_ + 1])) {
                advance();
                advance();
                while (!is_at_end() && is_digit(peek_char()))
                    advance();
            } else {
                break;
            }
        }
    }

    // exponent part: e[+-]DIGITS
    if (!is_at_end() && (peek_char() == 'e' || peek_char() == 'E')) {
        is_float = true;
        advance(); // e/E
        if (!is_at_end() && (peek_char() == '+' || peek_char() == '-'))
            advance();
        while (!is_at_end() && is_digit(peek_char()))
            advance();
        while (!is_at_end() && peek_char() == '_') {
            if (pos_ + 1 < len_ && is_digit(data_[pos_ + 1])) {
                advance();
                advance();
                while (!is_at_end() && is_digit(peek_char()))
                    advance();
            } else {
                break;
            }
        }
    }

    Token tok;
    tok.type   = is_float ? TokenType::kFloat : TokenType::kInteger;
    tok.text   = std::string(data_ + raw_start, pos_ - raw_start);
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

Lexer::Token Lexer::lex_hex_integer(size_t start_line, size_t start_col, size_t raw_start) {
    advance(); // '0'
    advance(); // 'x' or 'X'

    bool has_digits = false;
    while (!is_at_end() && is_hex_digit(peek_char())) {
        advance();
        has_digits = true;
        while (!is_at_end() && peek_char() == '_') {
            if (pos_ + 1 < len_ && is_hex_digit(data_[pos_ + 1])) {
                advance();
                advance();
            } else {
                break;
            }
        }
    }

    if (!has_digits)
        return error_token("expected hex digits", start_line, start_col);

    Token tok;
    tok.type   = TokenType::kInteger;
    tok.text   = std::string(data_ + raw_start, pos_ - raw_start);
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

Lexer::Token Lexer::lex_oct_integer(size_t start_line, size_t start_col, size_t raw_start) {
    advance(); // '0'
    advance(); // 'o' or 'O'

    bool has_digits = false;
    while (!is_at_end() && is_oct_digit(peek_char())) {
        advance();
        has_digits = true;
        while (!is_at_end() && peek_char() == '_') {
            if (pos_ + 1 < len_ && is_oct_digit(data_[pos_ + 1])) {
                advance();
                advance();
            } else {
                break;
            }
        }
    }

    if (!has_digits)
        return error_token("expected octal digits", start_line, start_col);

    Token tok;
    tok.type   = TokenType::kInteger;
    tok.text   = std::string(data_ + raw_start, pos_ - raw_start);
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

Lexer::Token Lexer::lex_bin_integer(size_t start_line, size_t start_col, size_t raw_start) {
    advance(); // '0'
    advance(); // 'b' or 'B'

    bool has_digits = false;
    while (!is_at_end() && is_bin_digit(peek_char())) {
        advance();
        has_digits = true;
        while (!is_at_end() && peek_char() == '_') {
            if (pos_ + 1 < len_ && is_bin_digit(data_[pos_ + 1])) {
                advance();
                advance();
            } else {
                break;
            }
        }
    }

    if (!has_digits)
        return error_token("expected binary digits", start_line, start_col);

    Token tok;
    tok.type   = TokenType::kInteger;
    tok.text   = std::string(data_ + raw_start, pos_ - raw_start);
    tok.line   = start_line;
    tok.column = start_col;
    return tok;
}

// ---------------------------------------------------------------------------
// Datetime lexing (unused standalone entry point)
// ---------------------------------------------------------------------------

Lexer::Token Lexer::lex_datetime(size_t start_line, size_t start_col) {
    // Datetime lexing is integrated into lex_number() via try_lex_datetime().
    // This standalone entry point is kept for interface compatibility.
    return error_token("unexpected datetime token", start_line, start_col);
}

} // namespace TOML
