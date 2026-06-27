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

#include "src/format_utils.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <sstream>

namespace toml {

std::string escape_toml_string(const std::string& s) {
    std::string result;
    result.reserve(s.size() + 2);
    result.push_back('"');
    for (unsigned char c : s) {
        switch (c) {
        case '"':  result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        case '\0': {
            char buf[8];
            std::snprintf(buf, sizeof(buf), "\\u%04x", 0);
            result += buf;
            break;
        }
        default:
            if (c < 0x20) {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                result += buf;
            } else {
                result.push_back(static_cast<char>(c));
            }
        }
    }
    result.push_back('"');
    return result;
}

std::string json_escape_string(const std::string& s) {
    std::string result;
    result.reserve(s.size() + 2);
    result.push_back('"');
    for (unsigned char c : s) {
        switch (c) {
        case '"':  result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default:
            if (c < 0x20) {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                result += buf;
            } else {
                result.push_back(static_cast<char>(c));
            }
        }
    }
    result.push_back('"');
    return result;
}

std::string escape_toml_key(const std::string& key) {
    // Check if key is a valid bare key
    if (key.empty()) {
        return escape_toml_string(key);
    }
    for (char c : key) {
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '-' || c == '_')) {
            return escape_toml_string(key);
        }
    }
    return key;
}

std::string format_float_toml(double v) {
    if (!std::isfinite(v)) {
        if (std::isnan(v)) return "nan";
        if (v > 0) return "inf";
        return "-inf";
    }
    std::ostringstream ss;
    ss.precision(std::numeric_limits<double>::max_digits10);
    ss << v;
    std::string s = ss.str();
    // TOML requires a decimal point or exponent for floats
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos &&
        s.find('E') == std::string::npos) {
        s += ".0";
    }
    return s;
}

std::string format_float_json(double v) {
    if (!std::isfinite(v)) {
        // JSON has no representation for inf/nan
        return "null";
    }
    std::ostringstream ss;
    ss.precision(std::numeric_limits<double>::max_digits10);
    ss << v;
    return ss.str();
}

} // namespace toml
