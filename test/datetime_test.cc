#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckDateYear(TOML::DateTime::Detail &dt, int year) { return dt.Year() == year; }
bool CheckDateMonth(TOML::DateTime::Detail &dt, int month) { return dt.Month() == month; }
bool CheckDateDay(TOML::DateTime::Detail &dt, int day) { return dt.Day() == day; }

bool CheckTimeHour(TOML::DateTime::Detail &dt, int hour) { return dt.Hour() == hour; }
bool CheckTimeMinute(TOML::DateTime::Detail &dt, int minute) { return dt.Minute() == minute; }
bool CheckTimeSecond(TOML::DateTime::Detail &dt, int second) { return dt.Second() == second; }
bool CheckTimeMicroSecond(TOML::DateTime::Detail &dt, int usecond) {
    return dt.MicroSecond() == usecond;
}

bool CheckGMTOffsetSecond(TOML::DateTime::Detail &dt, int second) {
    return dt.GMTOffset() == second;
}
TEST(DateTime, RFC3339Format) {
    std::string path = TEST_CASE_DIR "/ts1.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("odt1");
    ASSERT_EQ(n1.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27T07:32:00Z"));
    auto dt1 = n1.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    TOML::Node n2 = node.As<TOML::kTable>()->Get("odt2");
    ASSERT_EQ(n2.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27T00:32:00-07:00"));
    auto dt2 = n2.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt2, 1979));
    ASSERT_TRUE(CheckDateMonth(dt2, 5));
    ASSERT_TRUE(CheckDateDay(dt2, 27));
    ASSERT_TRUE(CheckTimeHour(dt2, 0));
    ASSERT_TRUE(CheckTimeMinute(dt2, 32));
    ASSERT_TRUE(CheckTimeSecond(dt2, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt2, 7 * 3600));

    TOML::Node n3 = node.As<TOML::kTable>()->Get("odt3");
    ASSERT_EQ(n3.As<TOML::kDateTime>()->RawString(),
              std::string("1979-05-27T00:32:00.999999-07:00"));
    auto dt3 = n3.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt3, 1979));
    ASSERT_TRUE(CheckDateMonth(dt3, 5));
    ASSERT_TRUE(CheckDateDay(dt3, 27));
    ASSERT_TRUE(CheckTimeHour(dt3, 0));
    ASSERT_TRUE(CheckTimeMinute(dt3, 32));
    ASSERT_TRUE(CheckTimeSecond(dt3, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt3, 7 * 3600));
    ASSERT_TRUE(CheckTimeMicroSecond(dt3, 999999));

    TOML::Node n4 = node.As<TOML::kTable>()->Get("odt4");
    ASSERT_EQ(n4.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27T00:32:00.01-07:00"));
    auto dt4 = n4.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt4, 1979));
    ASSERT_TRUE(CheckDateMonth(dt4, 5));
    ASSERT_TRUE(CheckDateDay(dt4, 27));
    ASSERT_TRUE(CheckTimeHour(dt4, 0));
    ASSERT_TRUE(CheckTimeMinute(dt4, 32));
    ASSERT_TRUE(CheckTimeSecond(dt4, 0));
    ASSERT_TRUE(CheckGMTOffsetSecond(dt4, 7 * 3600));
    ASSERT_TRUE(CheckTimeMicroSecond(dt4, 10 * 1000));
}

TEST(DateTime, RFC3339FormatEx) {
    std::string path = TEST_CASE_DIR "/ts2.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("odt4");
    ASSERT_EQ(n1.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27 07:32:00Z"));
    auto dt1 = n1.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));
}

TEST(DateTime, NoTimeZone) {
    std::string path = TEST_CASE_DIR "/ts3.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("ldt1");
    ASSERT_EQ(n1.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27T07:32:00"));
    auto dt1 = n1.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    TOML::Node n3 = node.As<TOML::kTable>()->Get("ldt2");
    ASSERT_EQ(n3.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27T00:32:00.999999"));
    auto dt3 = n3.As<TOML::kDateTime>()->Value();
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
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("ld1");
    ASSERT_EQ(n1.As<TOML::kDateTime>()->RawString(), std::string("1979-05-27"));
    auto dt1 = n1.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckDateYear(dt1, 1979));
    ASSERT_TRUE(CheckDateMonth(dt1, 5));
    ASSERT_TRUE(CheckDateDay(dt1, 27));
}

TEST(DateTime, Time) {
    std::string path = TEST_CASE_DIR "/ts5.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node n1 = node.As<TOML::kTable>()->Get("lt1");
    ASSERT_EQ(n1.As<TOML::kDateTime>()->RawString(), std::string("07:32:00"));
    auto dt1 = n1.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckTimeHour(dt1, 7));
    ASSERT_TRUE(CheckTimeMinute(dt1, 32));
    ASSERT_TRUE(CheckTimeSecond(dt1, 0));

    TOML::Node n2 = node.As<TOML::kTable>()->Get("lt2");
    ASSERT_EQ(n2.As<TOML::kDateTime>()->RawString(), std::string("00:32:00.999999"));
    auto dt3 = n2.As<TOML::kDateTime>()->Value();
    ASSERT_TRUE(CheckTimeHour(dt3, 0));
    ASSERT_TRUE(CheckTimeMinute(dt3, 32));
    ASSERT_TRUE(CheckTimeSecond(dt3, 0));
    ASSERT_TRUE(CheckTimeMicroSecond(dt3, 999999));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
