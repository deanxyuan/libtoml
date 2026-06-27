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

#include "toml/datetime.h"
#include <cstdio>

namespace TOML {

static void append_utc_offset(std::string& out, int32_t offset_seconds) {
    if (offset_seconds == 0) {
        out += 'Z';
        return;
    }
    char sign = '+';
    if (offset_seconds < 0) {
        sign = '-';
        offset_seconds = -offset_seconds;
    }
    int32_t hours = offset_seconds / 3600;
    int32_t minutes = (offset_seconds % 3600) / 60;
    char buf[8];
    std::snprintf(buf, sizeof(buf), "%c%02d:%02d", sign, hours, minutes);
    out += buf;
}

std::string DateTime::to_string() const {
    std::string result;

    // Date part
    if (has_year()) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%04d", year);
        result += buf;
    }
    if (has_month()) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "-%02d", month);
        result += buf;
    }
    if (has_day()) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "-%02d", day);
        result += buf;
    }

    // Time part
    if (has_hour()) {
        if (has_year()) {
            result += time_separator;
        }
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minute, second);
        result += buf;
    }

    // Microseconds
    if (has_microsecond() && microsecond > 0) {
        char buf[16];
        int digits = microsecond_digits > 0 && microsecond_digits <= 6 ? microsecond_digits : 6;
        // Strip trailing zeros but keep at least 'digits' places
        uint32_t us = microsecond;
        // Truncate to original digit count
        for (int i = 6; i > digits; i--)
            us /= 10;
        std::snprintf(buf, sizeof(buf), ".%0*u", digits, us);
        result += buf;
    }

    // UTC offset
    if (has_utc_offset()) {
        append_utc_offset(result, utc_offset);
    }

    return result;
}

std::string DateTime::to_toml() const {
    // Same format as to_string() -- TOML datetime literals are ISO 8601
    return to_string();
}

} // namespace TOML
