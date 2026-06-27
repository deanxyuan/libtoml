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

#include "utf8.h"

#include <cstring>

namespace TOML {
namespace utf8 {

// ---- helpers ----

bool is_valid_codepoint(uint32_t codepoint) {
    // Maximum valid Unicode codepoint is U+10FFFF
    return codepoint <= 0x10FFFF;
}

bool is_scalar(uint32_t codepoint) {
    // Surrogate range U+D800..U+DFFF are not scalar values
    return codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF);
}

size_t sequence_length(char first_byte) {
    uint8_t b = static_cast<uint8_t>(first_byte);
    if (b < 0x80)
        return 1; // 0xxxxxxx
    if ((b & 0xE0) == 0xC0)
        return 2; // 110xxxxx
    if ((b & 0xF0) == 0xE0)
        return 3; // 1110xxxx
    if ((b & 0xF8) == 0xF0)
        return 4; // 11110xxx
    return 0;     // invalid first byte
}

// ---- encode ----

size_t encode(uint32_t codepoint, char* buf) {
    if (codepoint <= 0x7F) {
        // 1 byte: 0xxxxxxx
        if (buf) {
            buf[0] = static_cast<char>(codepoint);
        }
        return 1;
    }
    if (codepoint <= 0x7FF) {
        // 2 bytes: 110xxxxx 10xxxxxx
        if (buf) {
            buf[0] = static_cast<char>(0xC0 | (codepoint >> 6));
            buf[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        return 2;
    }
    if (codepoint <= 0xFFFF) {
        // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        if (buf) {
            buf[0] = static_cast<char>(0xE0 | (codepoint >> 12));
            buf[1] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buf[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        return 3;
    }
    if (codepoint <= 0x10FFFF) {
        // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (buf) {
            buf[0] = static_cast<char>(0xF0 | (codepoint >> 18));
            buf[1] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            buf[2] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buf[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        return 4;
    }
    // Invalid codepoint
    return 0;
}

// ---- decode ----

DecodeResult decode(const char* data, size_t len) {
    DecodeResult result = {0, 0};

    if (len == 0) {
        return result;
    }

    uint8_t b0 = static_cast<uint8_t>(data[0]);
    size_t seq_len = sequence_length(data[0]);

    if (seq_len == 0) {
        return result; // invalid first byte
    }
    if (seq_len > len) {
        return result; // not enough data
    }

    // Validate continuation bytes: each must be 10xxxxxx
    for (size_t i = 1; i < seq_len; ++i) {
        if ((static_cast<uint8_t>(data[i]) & 0xC0) != 0x80) {
            return result;
        }
    }

    uint32_t codepoint = 0;

    switch (seq_len) {
    case 1:
        codepoint = b0;
        break;
    case 2:
        codepoint = b0 & 0x1F;
        break;
    case 3:
        codepoint = b0 & 0x0F;
        break;
    case 4:
        codepoint = b0 & 0x07;
        break;
    }

    for (size_t i = 1; i < seq_len; ++i) {
        codepoint = (codepoint << 6) | (static_cast<uint8_t>(data[i]) & 0x3F);
    }

    // Check for overlong encoding
    // 1 byte:  U+0000..U+007F
    // 2 bytes: U+0080..U+07FF
    // 3 bytes: U+0800..U+FFFF
    // 4 bytes: U+10000..U+10FFFF
    switch (seq_len) {
    case 2:
        if (codepoint < 0x80)
            return result;
        break;
    case 3:
        if (codepoint < 0x800)
            return result;
        break;
    case 4:
        if (codepoint < 0x10000)
            return result;
        break;
    }

    // Check for surrogate pairs (U+D800..U+DFFF)
    if (!is_scalar(codepoint)) {
        return result;
    }

    result.codepoint = codepoint;
    result.consumed = seq_len;
    return result;
}

// ---- validate ----

bool validate(const char* data, size_t len) {
    size_t pos = 0;
    while (pos < len) {
        DecodeResult r = decode(data + pos, len - pos);
        if (r.consumed == 0) {
            return false;
        }
        pos += r.consumed;
    }
    return true;
}

bool validate(const std::string& str) {
    return validate(str.data(), str.size());
}

// ---- escape_to_utf8 ----

bool escape_to_utf8(uint32_t codepoint, std::string& result) {
    if (!is_scalar(codepoint)) {
        return false;
    }
    char buf[4];
    size_t n = encode(codepoint, buf);
    if (n == 0) {
        return false;
    }
    result.append(buf, n);
    return true;
}

} // namespace utf8
} // namespace TOML
