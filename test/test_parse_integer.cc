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

#include "toml/toml.h"
#include "util/testutil.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Integer, DecimalInteger) {
    std::string path = TEST_CASE_DIR "/int0.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& n1 = node.as_table().at("int1");
    ASSERT_EQ(n1.as_integer(), 99);

    const auto& n2 = node.as_table().at("int2");
    ASSERT_EQ(n2.as_integer(), 42);

    const auto& n3 = node.as_table().at("int3");
    ASSERT_EQ(n3.as_integer(), 0);

    const auto& n4 = node.as_table().at("int4");
    ASSERT_EQ(n4.as_integer(), -17);

    const auto& n5 = node.as_table().at("int5");
    ASSERT_EQ(n5.as_integer(), 1000);

    const auto& n6 = node.as_table().at("int6");
    ASSERT_EQ(n6.as_integer(), 5349221);

    const auto& n7 = node.as_table().at("int7");
    ASSERT_EQ(n7.as_integer(), 5349221);

    const auto& n8 = node.as_table().at("int8");
    ASSERT_EQ(n8.as_integer(), 12345);
}

TEST(Integer, OtherDecimalInteger) {
    std::string path = TEST_CASE_DIR "/int1.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& n1 = node.as_table().at("hex1");
    ASSERT_EQ(n1.as_integer(), static_cast<int64_t>(0xDEADBEEF));

    const auto& n2 = node.as_table().at("hex2");
    ASSERT_EQ(n2.as_integer(), static_cast<int64_t>(0xdeadbeef));

    const auto& n3 = node.as_table().at("hex3");
    ASSERT_EQ(n3.as_integer(), static_cast<int64_t>(0xdeadbeef));

    const auto& n4 = node.as_table().at("oct1");
    ASSERT_EQ(n4.as_integer(), 01234567);

    const auto& n5 = node.as_table().at("oct2");
    ASSERT_EQ(n5.as_integer(), 0755);

    const auto& n6 = node.as_table().at("bin1");
    ASSERT_EQ(n6.as_integer(), 0b11010110);
}


RUN_ALL_TESTS()
