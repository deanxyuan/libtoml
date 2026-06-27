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

#ifndef TOML_DATETIME_H_
#define TOML_DATETIME_H_

#include <cstdint>
#include <string>

namespace toml {

struct DateTime {
    uint16_t year;
    uint8_t month, day;
    uint8_t hour, minute, second;
    uint32_t microsecond;
    uint8_t microsecond_digits; // original decimal digits (preserves precision in to_string)
    char time_separator;        // original separator: 'T' or ' '
    int32_t utc_offset;         // seconds, 0 means not set

    enum Fields : uint16_t {
        kNone        = 0,
        kYear        = 1 << 0,
        kMonth       = 1 << 1,
        kDay         = 1 << 2,
        kHour        = 1 << 3,
        kMinute      = 1 << 4,
        kSecond      = 1 << 5,
        kMicrosecond = 1 << 6,
        kUtcOffset   = 1 << 7,
        kSpecific    = 1 << 8,
    };
    uint16_t present;

    DateTime()
        : year(0), month(0), day(0), hour(0), minute(0), second(0),
          microsecond(0), microsecond_digits(0), time_separator('T'),
          utc_offset(0), present(kNone) {}

    bool has_year() const { return present & kYear; }
    bool has_month() const { return present & kMonth; }
    bool has_day() const { return present & kDay; }
    bool has_hour() const { return present & kHour; }
    bool has_minute() const { return present & kMinute; }
    bool has_second() const { return present & kSecond; }
    bool has_microsecond() const { return present & kMicrosecond; }
    bool has_utc_offset() const { return present & kUtcOffset; }
    bool is_specific() const { return present & kSpecific; }

    bool is_date_only() const {
        return has_year() && has_month() && has_day() && !has_hour();
    }
    bool is_time_only() const { return !has_year() && has_hour(); }

    std::string to_string() const;
    std::string to_toml() const;
};

} // namespace toml

#endif // TOML_DATETIME_H_
