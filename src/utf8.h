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

#ifndef TOML_UTF8_H_
#define TOML_UTF8_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace toml {
namespace utf8 {

// Encode a Unicode codepoint to UTF-8, returns the number of bytes written.
// If buf is nullptr, only calculates the required byte count.
size_t encode(uint32_t codepoint, char* buf);

// Decode a UTF-8 sequence, returns the codepoint and number of bytes consumed.
// On failure, codepoint is 0 and consumed is 0.
struct DecodeResult {
    uint32_t codepoint;
    size_t consumed;
};
DecodeResult decode(const char* data, size_t len);

// Validate a UTF-8 string.
bool validate(const char* data, size_t len);
bool validate(const std::string& str);

// Check if a codepoint is a valid Unicode codepoint.
bool is_valid_codepoint(uint32_t codepoint);

// Check if a codepoint is a scalar value (not a surrogate pair).
bool is_scalar(uint32_t codepoint);

// Get the UTF-8 sequence length from the first byte.
size_t sequence_length(char first_byte);

// Convert a \uXXXX or \UXXXXXXXX escape sequence to a UTF-8 string.
// Returns true on success, result contains the UTF-8 bytes.
// Returns false if the codepoint is invalid.
bool escape_to_utf8(uint32_t codepoint, std::string& result);

} // namespace utf8
} // namespace toml

#endif // TOML_UTF8_H_
