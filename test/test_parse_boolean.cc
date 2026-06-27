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

TEST(Boolean, BasicTest) {
    std::string path = TEST_CASE_DIR "/bool1.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("bool1");
    ASSERT_TRUE(n1.as_bool());
    const auto& n2 = node.as_table().at("bool2");
    ASSERT_FALSE(n2.as_bool());
}

TEST(Boolean, ParseFromString) {
    // Valid booleans via parse_string
    auto r1 = toml::parse_string("v = true");
    ASSERT_TRUE(r1.ok()) << r1.error.to_string();
    ASSERT_TRUE(r1.value.as_table().at("v").is_boolean());
    ASSERT_TRUE(r1.value.as_table().at("v").as_bool());

    auto r2 = toml::parse_string("v = false");
    ASSERT_TRUE(r2.ok()) << r2.error.to_string();
    ASSERT_TRUE(r2.value.as_table().at("v").is_boolean());
    ASSERT_FALSE(r2.value.as_table().at("v").as_bool());
}

TEST(Boolean, InvalidTrueCasing) {
    // TOML spec: only lowercase "true" is a boolean.
    // "True" and "TRUE" are NOT valid booleans.
    // Current parser treats them as bare strings (kString).
    auto r1 = toml::parse_string("v = True");
    ASSERT_TRUE(r1.ok());
    ASSERT_FALSE(r1.value.as_table().at("v").is_boolean())
        << "True should not be parsed as boolean";

    auto r2 = toml::parse_string("v = TRUE");
    ASSERT_TRUE(r2.ok());
    ASSERT_FALSE(r2.value.as_table().at("v").is_boolean())
        << "TRUE should not be parsed as boolean";
}

TEST(Boolean, InvalidFalseCasing) {
    // TOML spec: only lowercase "false" is a boolean.
    auto r1 = toml::parse_string("v = False");
    ASSERT_TRUE(r1.ok());
    ASSERT_FALSE(r1.value.as_table().at("v").is_boolean())
        << "False should not be parsed as boolean";

    auto r2 = toml::parse_string("v = FALSE");
    ASSERT_TRUE(r2.ok());
    ASSERT_FALSE(r2.value.as_table().at("v").is_boolean())
        << "FALSE should not be parsed as boolean";
}

TEST(Boolean, InvalidBoolWords) {
    // "yes" and "no" are not valid TOML booleans
    auto r1 = toml::parse_string("v = yes");
    ASSERT_TRUE(r1.ok());
    ASSERT_FALSE(r1.value.as_table().at("v").is_boolean())
        << "yes should not be parsed as boolean";

    auto r2 = toml::parse_string("v = no");
    ASSERT_TRUE(r2.ok());
    ASSERT_FALSE(r2.value.as_table().at("v").is_boolean())
        << "no should not be parsed as boolean";
}

TEST(Boolean, InvalidBoolNumbers) {
    // 1 and 0 are integers, not booleans in TOML
    auto r1 = toml::parse_string("v = 1");
    ASSERT_TRUE(r1.ok());
    ASSERT_FALSE(r1.value.as_table().at("v").is_boolean());
    ASSERT_TRUE(r1.value.as_table().at("v").is_integer());

    auto r2 = toml::parse_string("v = 0");
    ASSERT_TRUE(r2.ok());
    ASSERT_FALSE(r2.value.as_table().at("v").is_boolean());
    ASSERT_TRUE(r2.value.as_table().at("v").is_integer());
}

RUN_ALL_TESTS()
