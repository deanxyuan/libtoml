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

TEST(Error, ErrorLocation) {
    // Duplicate key should produce an error with location info
    std::string input = R"(
key = 1
key = 2
)";
    auto error = testutil::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_GT(error.location.line, 0u);

    // Invalid syntax on a specific line
    std::string input2 = R"(name = "valid"
broken !!! syntax
)";
    auto error2 = testutil::ParseExpectFail(input2);
    ASSERT_FALSE(error2.ok());
    ASSERT_GT(error2.location.line, 0u);

    // Missing value
    std::string input3 = "key = ";
    auto error3 = testutil::ParseExpectFail(input3);
    ASSERT_FALSE(error3.ok());

    // Unclosed bracket
    std::string input4 = "arr = [1, 2, 3";
    auto error4 = testutil::ParseExpectFail(input4);
    ASSERT_FALSE(error4.ok());
}

TEST(Error, ErrorMessage) {
    // Error should contain a non-empty message
    std::string input = "key = ";
    auto error = testutil::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_FALSE(error.message.empty());

    // to_string should produce a non-empty result
    std::string str = error.to_string();
    ASSERT_FALSE(str.empty());

    // Duplicate key error
    std::string input2 = R"(
key = 1
key = 2
)";
    auto error2 = testutil::ParseExpectFail(input2);
    ASSERT_FALSE(error2.ok());
    ASSERT_FALSE(error2.message.empty());

    // Invalid value
    std::string input3 = "key = !!!";
    auto error3 = testutil::ParseExpectFail(input3);
    ASSERT_FALSE(error3.ok());
    ASSERT_FALSE(error3.message.empty());

    // ParseResult::ok() consistency
    auto result = TOML::parse_string(input);
    ASSERT_FALSE(result.ok());
    ASSERT_TRUE(result.error.ok() == false);

    // Successful parse should have ok() == true and empty message
    std::string valid = "key = 42";
    auto result_ok = TOML::parse_string(valid);
    ASSERT_TRUE(result_ok.ok());
    ASSERT_TRUE(result_ok.error.message.empty());
    ASSERT_EQ(result_ok.error.location.line, 0u);
    ASSERT_EQ(result_ok.error.location.column, 0u);
}

RUN_ALL_TESTS()
