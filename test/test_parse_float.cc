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

TEST(Float, float1) {
    std::string path = TEST_CASE_DIR "/float1.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("flt1");
    ASSERT_EQ(n1.as_float(), 1.0);
    const auto& n2 = node.as_table().at("flt2");
    ASSERT_EQ(n2.as_float(), 3.1415);
    const auto& n3 = node.as_table().at("flt3");
    ASSERT_EQ(n3.as_float(), -0.01);
    const auto& n4 = node.as_table().at("flt4");
    ASSERT_EQ(n4.as_float(), 5e+22);
    const auto& n5 = node.as_table().at("flt5");
    ASSERT_EQ(n5.as_float(), 1e06);
    const auto& n6 = node.as_table().at("flt6");
    ASSERT_EQ(n6.as_float(), -2E-2);
    const auto& n7 = node.as_table().at("flt7");
    ASSERT_EQ(n7.as_float(), 6.626e-34);
}

TEST(Float, float2) {
    std::string path = TEST_CASE_DIR "/float2.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Float, float3) {
    std::string path = TEST_CASE_DIR "/float3.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Float, float4) {
    std::string path = TEST_CASE_DIR "/float4.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Float, float5) {
    std::string path = TEST_CASE_DIR "/float5.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("flt8");
    ASSERT_EQ(n1.as_float(), 224617.445991228);
}

TEST(Float, float6) {
    std::string path = TEST_CASE_DIR "/float6.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("sf1");
    ASSERT_TRUE(std::isinf(n1.as_float()));
    ASSERT_EQ(n1.as_float(), INFINITY);

    const auto& n2 = node.as_table().at("sf2");
    ASSERT_TRUE(std::isinf(n2.as_float()));
    ASSERT_EQ(n2.as_float(), INFINITY);

    const auto& n3 = node.as_table().at("sf3");
    ASSERT_TRUE(std::isinf(n3.as_float()));
    ASSERT_EQ(n3.as_float(), INFINITY * -1);

    const auto& n4 = node.as_table().at("sf4");
    ASSERT_TRUE(std::isnan(n4.as_float()));

    const auto& n5 = node.as_table().at("sf5");
    ASSERT_TRUE(std::isnan(n5.as_float()));

    const auto& n6 = node.as_table().at("sf6");
    ASSERT_TRUE(std::isnan(n6.as_float()));
}


RUN_ALL_TESTS()
