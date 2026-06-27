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

#include "util/testutil.h"

TEST(DateTime, DateOnly) {
    TOML::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                 TOML::DateTime::kDay;

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
    TOML::DateTime dt;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 0;
    dt.present = TOML::DateTime::kHour | TOML::DateTime::kMinute |
                 TOML::DateTime::kSecond;

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

TEST(DateTime, DateTime) {
    TOML::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 45;
    dt.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                 TOML::DateTime::kDay | TOML::DateTime::kHour |
                 TOML::DateTime::kMinute | TOML::DateTime::kSecond;

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
    TOML::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.hour = 14;
    dt.minute = 30;
    dt.second = 0;
    dt.microsecond = 123456;
    dt.utc_offset = 8 * 3600; // UTC+8
    dt.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                 TOML::DateTime::kDay | TOML::DateTime::kHour |
                 TOML::DateTime::kMinute | TOML::DateTime::kSecond |
                 TOML::DateTime::kMicrosecond | TOML::DateTime::kUtcOffset;

    ASSERT_TRUE(dt.has_utc_offset());
    ASSERT_TRUE(dt.has_microsecond());
    ASSERT_EQ(dt.utc_offset, 8 * 3600);
    ASSERT_EQ(dt.microsecond, 123456u);

    // Negative offset
    TOML::DateTime dt_neg;
    dt_neg.utc_offset = -5 * 3600; // UTC-5
    dt_neg.present = TOML::DateTime::kUtcOffset;
    ASSERT_TRUE(dt_neg.has_utc_offset());
    ASSERT_EQ(dt_neg.utc_offset, -5 * 3600);
}

TEST(DateTime, ToString) {
    // Date only
    TOML::DateTime dt_date;
    dt_date.year = 2023;
    dt_date.month = 6;
    dt_date.day = 15;
    dt_date.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                      TOML::DateTime::kDay;
    std::string s1 = dt_date.to_string();
    ASSERT_FALSE(s1.empty());

    // Time only
    TOML::DateTime dt_time;
    dt_time.hour = 14;
    dt_time.minute = 30;
    dt_time.second = 45;
    dt_time.present = TOML::DateTime::kHour | TOML::DateTime::kMinute |
                      TOML::DateTime::kSecond;
    std::string s2 = dt_time.to_string();
    ASSERT_FALSE(s2.empty());

    // Full datetime
    TOML::DateTime dt_full;
    dt_full.year = 2023;
    dt_full.month = 6;
    dt_full.day = 15;
    dt_full.hour = 14;
    dt_full.minute = 30;
    dt_full.second = 45;
    dt_full.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                      TOML::DateTime::kDay | TOML::DateTime::kHour |
                      TOML::DateTime::kMinute | TOML::DateTime::kSecond;
    std::string s3 = dt_full.to_string();
    ASSERT_FALSE(s3.empty());

    // to_toml
    std::string t1 = dt_date.to_toml();
    ASSERT_FALSE(t1.empty());
    std::string t2 = dt_full.to_toml();
    ASSERT_FALSE(t2.empty());
}

RUN_ALL_TESTS()
