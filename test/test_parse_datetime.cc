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

#include "toml/toml.h"
#include "util/testutil.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckDateYear(const toml::DateTime &dt, int year) { return dt.year == year; }
bool CheckDateMonth(const toml::DateTime &dt, int month) { return dt.month == month; }
bool CheckDateDay(const toml::DateTime &dt, int day) { return dt.day == day; }

bool CheckTimeHour(const toml::DateTime &dt, int hour) { return dt.hour == hour; }
bool CheckTimeMinute(const toml::DateTime &dt, int minute) { return dt.minute == minute; }
bool CheckTimeSecond(const toml::DateTime &dt, int second) { return dt.second == second; }
bool CheckTimeMicroSecond(const toml::DateTime &dt, int usecond) {
    return dt.microsecond == static_cast<uint32_t>(usecond);
}

bool CheckGMTOffsetSecond(const toml::DateTime &dt, int second) {
    return dt.utc_offset == second;
}

TEST(DateTime, RFC3339Format) {
    std::string path = TEST_CASE_DIR "/ts1.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("odt1");
    ASSERT_EQ(n1.as_datetime().to_string(), std::string("1979-05-27T07:32:00Z"));
    const auto& dt1 = n1.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    const auto& n2 = node.as_table().at("odt2");
    ASSERT_EQ(n2.as_datetime().to_string(), std::string("1979-05-27T00:32:00-07:00"));
    const auto& dt2 = n2.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt2, 1979));
    ASSERT_TRUE(CheckDateMonth(dt2, 5));
    ASSERT_TRUE(CheckDateDay(dt2, 27));
    ASSERT_TRUE(CheckTimeHour(dt2, 0));
    ASSERT_TRUE(CheckTimeMinute(dt2, 32));
    ASSERT_TRUE(CheckTimeSecond(dt2, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt2, -7 * 3600));

    const auto& n3 = node.as_table().at("odt3");
    ASSERT_EQ(n3.as_datetime().to_string(),
              std::string("1979-05-27T00:32:00.999999-07:00"));
    const auto& dt3 = n3.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt3, 1979));
    ASSERT_TRUE(CheckDateMonth(dt3, 5));
    ASSERT_TRUE(CheckDateDay(dt3, 27));
    ASSERT_TRUE(CheckTimeHour(dt3, 0));
    ASSERT_TRUE(CheckTimeMinute(dt3, 32));
    ASSERT_TRUE(CheckTimeSecond(dt3, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt3, -7 * 3600));
    ASSERT_TRUE(CheckTimeMicroSecond(dt3, 999999));

    const auto& n4 = node.as_table().at("odt4");
    ASSERT_EQ(n4.as_datetime().to_string(), std::string("1979-05-27T00:32:00.01-07:00"));
    const auto& dt4 = n4.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt4, 1979));
    ASSERT_TRUE(CheckDateMonth(dt4, 5));
    ASSERT_TRUE(CheckDateDay(dt4, 27));
    ASSERT_TRUE(CheckTimeHour(dt4, 0));
    ASSERT_TRUE(CheckTimeMinute(dt4, 32));
    ASSERT_TRUE(CheckTimeSecond(dt4, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt4, -7 * 3600));
    ASSERT_TRUE(CheckTimeMicroSecond(dt4, 10 * 1000));
}

TEST(DateTime, RFC3339FormatEx) {
    std::string path = TEST_CASE_DIR "/ts2.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("odt4");
    ASSERT_EQ(n1.as_datetime().to_string(), std::string("1979-05-27 07:32:00Z"));
    const auto& dt1 = n1.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));
}

TEST(DateTime, NoTimeZone) {
    std::string path = TEST_CASE_DIR "/ts3.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("ldt1");
    ASSERT_EQ(n1.as_datetime().to_string(), std::string("1979-05-27T07:32:00"));
    const auto& dt1 = n1.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    const auto& n3 = node.as_table().at("ldt2");
    ASSERT_EQ(n3.as_datetime().to_string(), std::string("1979-05-27T00:32:00.999999"));
    const auto& dt3 = n3.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt3, 1979));
    ASSERT_TRUE(CheckDateMonth(dt3, 5));
    ASSERT_TRUE(CheckDateDay(dt3, 27));
    ASSERT_TRUE(CheckTimeHour(dt3, 0));
    ASSERT_TRUE(CheckTimeMinute(dt3, 32));
    ASSERT_TRUE(CheckTimeSecond(dt3, 0));
    ASSERT_TRUE(CheckTimeMicroSecond(dt3, 999999));
}

TEST(DateTime, Date) {
    std::string path = TEST_CASE_DIR "/ts4.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("ld1");
    ASSERT_EQ(n1.as_datetime().to_string(), std::string("1979-05-27"));
    const auto& dt1 = n1.as_datetime();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
}

TEST(DateTime, Time) {
    std::string path = TEST_CASE_DIR "/ts5.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("lt1");
    ASSERT_EQ(n1.as_datetime().to_string(), std::string("07:32:00"));
    const auto& dt1 = n1.as_datetime();
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    const auto& n2 = node.as_table().at("lt2");
    ASSERT_EQ(n2.as_datetime().to_string(), std::string("00:32:00.999999"));
    const auto& dt3 = n2.as_datetime();
    ASSERT_TRUE(CheckTimeHour(dt3, 0));
    ASSERT_TRUE(CheckTimeMinute(dt3, 32));
    ASSERT_TRUE(CheckTimeSecond(dt3, 0));
    ASSERT_TRUE(CheckTimeMicroSecond(dt3, 999999));
}

// ===== Negative tests: invalid datetime field ranges =====
// Parser now validates datetime field ranges per RFC 3339.

TEST(DateTime, InvalidMonthZero) {
    auto r = toml::parse_string("v = 2023-00-15T14:30:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, InvalidMonth13) {
    auto r = toml::parse_string("v = 2023-13-15T14:30:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, InvalidDayZero) {
    auto r = toml::parse_string("v = 2023-06-00T14:30:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, InvalidDay32) {
    auto r = toml::parse_string("v = 2023-06-32T14:30:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, InvalidHour25) {
    auto r = toml::parse_string("v = 2023-06-15T25:30:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, InvalidMinute60) {
    auto r = toml::parse_string("v = 2023-06-15T14:60:45");
    ASSERT_FALSE(r.ok());
}

TEST(DateTime, LeapSecond) {
    // Second = 60 is valid per RFC 3339 (leap second)
    auto r = toml::parse_string("v = 2023-06-15T14:30:60");
    ASSERT_TRUE(r.ok()) << r.error.to_string();
    ASSERT_EQ(r.value.as_table().at("v").as_datetime().second, 60);
}

RUN_ALL_TESTS()
