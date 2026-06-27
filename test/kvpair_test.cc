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

TEST(KVPair, Basic) {
    std::string path = TEST_CASE_DIR "/kvpair0.toml";
    auto result = TOML::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& n1 = node.as_table().at("key");
    ASSERT_EQ(n1.as_string(), std::string("value"));
}

TEST(KVPair, Inlvaid1) {
    std::string path = TEST_CASE_DIR "/kvpair1.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(KVPair, Inlvaid2) {
    std::string path = TEST_CASE_DIR "/kvpair2.toml";
    auto result = TOML::parse_file(path);
    ASSERT_FALSE(result.ok());
}


RUN_ALL_TESTS()
