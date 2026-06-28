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

#include "util/testutil.h"

TEST(DateTime, DateOnly) {
    toml::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                 toml::DateTime::kDay;

    ASSERT_TRUE(dt.is_date_only());
    ASSERT_FALSE(dt.is_time_only());
    ASSERT_TRUE(dt.has_year());
    ASSERT_TRUE(dt.has_month());
    ASSERT_TRUE(dt.has_day());
    ASSERT_FALSE(dt.has_hour());
    ASSERT_FALSE(dt.has_minute());
    ASSERT_FALSE(dt.has_second());
    ASSERT_FALSE(dt.has_microsecond());
    ASSERT_FALSE(dt.has_utc_offset());

    ASSERT_EQ(dt.year, 2023);
    ASSERT_EQ(dt.month, 6);
    ASSERT_EQ(dt.day, 15);
}

TEST(DateTime, TimeOnly) {
    toml::DateTime dt;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 0;
    dt.present = toml::DateTime::kHour | toml::DateTime::kMinute |
                 toml::DateTime::kSecond;

    ASSERT_FALSE(dt.is_date_only());
    ASSERT_TRUE(dt.is_time_only());
    ASSERT_FALSE(dt.has_year());
    ASSERT_FALSE(dt.has_month());
    ASSERT_FALSE(dt.has_day());
    ASSERT_TRUE(dt.has_hour());
    ASSERT_TRUE(dt.has_minute());
    ASSERT_TRUE(dt.has_second());

    ASSERT_EQ(dt.hour, 14);
    ASSERT_EQ(dt.minute, 30);
    ASSERT_EQ(dt.second, 0);
}

TEST(DateTime, DateTimeFull) {
    toml::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 45;
    dt.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                 toml::DateTime::kDay | toml::DateTime::kHour |
                 toml::DateTime::kMinute | toml::DateTime::kSecond;

    ASSERT_FALSE(dt.is_date_only());
    ASSERT_FALSE(dt.is_time_only());
    ASSERT_TRUE(dt.has_year());
    ASSERT_TRUE(dt.has_hour());

    ASSERT_EQ(dt.year, 2023);
    ASSERT_EQ(dt.month, 6);
    ASSERT_EQ(dt.day, 15);
    ASSERT_EQ(dt.hour, 14);
    ASSERT_EQ(dt.minute, 30);
    ASSERT_EQ(dt.second, 45);
}

TEST(DateTime, DateTimeWithOffset) {
    toml::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 0;
    dt.microsecond = 123456;
    dt.utc_offset = 8 * 3600; // UTC+8
    dt.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                 toml::DateTime::kDay | toml::DateTime::kHour |
                 toml::DateTime::kMinute | toml::DateTime::kSecond |
                 toml::DateTime::kMicrosecond | toml::DateTime::kUtcOffset;

    ASSERT_TRUE(dt.has_utc_offset());
    ASSERT_TRUE(dt.has_microsecond());
    ASSERT_EQ(dt.utc_offset, 8 * 3600);
    ASSERT_EQ(dt.microsecond, 123456u);

    // Negative offset
    toml::DateTime dt_neg;
    dt_neg.utc_offset = -5 * 3600; // UTC-5
    dt_neg.present = toml::DateTime::kUtcOffset;
    ASSERT_TRUE(dt_neg.has_utc_offset());
    ASSERT_EQ(dt_neg.utc_offset, -5 * 3600);
}

TEST(DateTime, ToString) {
    // Date only: YYYY-MM-DD
    toml::DateTime dt_date;
    dt_date.year = 2023;
    dt_date.month = 6;
    dt_date.day = 15;
    dt_date.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                      toml::DateTime::kDay;
    ASSERT_EQ(dt_date.to_string(), "2023-06-15");
    ASSERT_EQ(dt_date.to_toml(), "2023-06-15");

    // Time only: HH:MM:SS
    toml::DateTime dt_time;
    dt_time.hour = 14;
    dt_time.minute = 30;
    dt_time.second = 45;
    dt_time.present = toml::DateTime::kHour | toml::DateTime::kMinute |
                      toml::DateTime::kSecond;
    ASSERT_EQ(dt_time.to_string(), "14:30:45");
    ASSERT_EQ(dt_time.to_toml(), "14:30:45");

    // Full datetime: YYYY-MM-DDTHH:MM:SS
    toml::DateTime dt_full;
    dt_full.year = 2023;
    dt_full.month = 6;
    dt_full.day = 15;
    dt_full.hour = 14;
    dt_full.minute = 30;
    dt_full.second = 45;
    dt_full.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                      toml::DateTime::kDay | toml::DateTime::kHour |
                      toml::DateTime::kMinute | toml::DateTime::kSecond;
    ASSERT_EQ(dt_full.to_string(), "2023-06-15T14:30:45");
    ASSERT_EQ(dt_full.to_toml(), "2023-06-15T14:30:45");

    // With microseconds
    toml::DateTime dt_us;
    dt_us.year = 2023;
    dt_us.month = 1;
    dt_us.day = 1;
    dt_us.hour = 0;
    dt_us.minute = 0;
    dt_us.second = 0;
    dt_us.microsecond = 123456;
    dt_us.microsecond_digits = 6;
    dt_us.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                    toml::DateTime::kDay | toml::DateTime::kHour |
                    toml::DateTime::kMinute | toml::DateTime::kSecond |
                    toml::DateTime::kMicrosecond;
    std::string s_us = dt_us.to_string();
    ASSERT_TRUE(s_us.find(".123456") != std::string::npos) << "got: " << s_us;

    // With UTC offset Z
    toml::DateTime dt_z;
    dt_z.year = 1979;
    dt_z.month = 5;
    dt_z.day = 27;
    dt_z.hour = 7;
    dt_z.minute = 32;
    dt_z.second = 0;
    dt_z.utc_offset = 0;
    dt_z.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                   toml::DateTime::kDay | toml::DateTime::kHour |
                   toml::DateTime::kMinute | toml::DateTime::kSecond |
                   toml::DateTime::kUtcOffset;
    ASSERT_EQ(dt_z.to_string(), "1979-05-27T07:32:00Z");

    // With positive UTC offset
    toml::DateTime dt_plus;
    dt_plus.year = 2023;
    dt_plus.month = 6;
    dt_plus.day = 15;
    dt_plus.hour = 14;
    dt_plus.minute = 30;
    dt_plus.second = 0;
    dt_plus.utc_offset = 8 * 3600;
    dt_plus.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                      toml::DateTime::kDay | toml::DateTime::kHour |
                      toml::DateTime::kMinute | toml::DateTime::kSecond |
                      toml::DateTime::kUtcOffset;
    ASSERT_EQ(dt_plus.to_string(), "2023-06-15T14:30:00+08:00");

    // With negative UTC offset
    toml::DateTime dt_minus;
    dt_minus.year = 1979;
    dt_minus.month = 5;
    dt_minus.day = 27;
    dt_minus.hour = 0;
    dt_minus.minute = 32;
    dt_minus.second = 0;
    dt_minus.utc_offset = -7 * 3600;
    dt_minus.present = toml::DateTime::kYear | toml::DateTime::kMonth |
                       toml::DateTime::kDay | toml::DateTime::kHour |
                       toml::DateTime::kMinute | toml::DateTime::kSecond |
                       toml::DateTime::kUtcOffset;
    ASSERT_EQ(dt_minus.to_string(), "1979-05-27T00:32:00-07:00");
}

RUN_ALL_TESTS()
