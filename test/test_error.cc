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

TEST(Error, DuplicateKey) {
    std::string input = "\nkey = 1\nkey = 2\n";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 3u);
    ASSERT_EQ(error.location.column, 8u);
    ASSERT_TRUE(error.message.find("duplicate key") != std::string::npos)
        << "got: " << error.message;

    std::string str = error.to_string();
    ASSERT_TRUE(str.find("line 3") != std::string::npos);
    ASSERT_TRUE(str.find("column 8") != std::string::npos);
    ASSERT_TRUE(str.find("duplicate key") != std::string::npos);
}

TEST(Error, SyntaxError) {
    std::string input = "name = \"valid\"\nbroken !!! syntax\n";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 2u);
    ASSERT_EQ(error.location.column, 8u);
    ASSERT_FALSE(error.message.empty());
    ASSERT_TRUE(error.message.find("expected '='") != std::string::npos)
        << "got: " << error.message;
}

TEST(Error, MissingValue) {
    std::string input = "key = ";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 1u);
    ASSERT_EQ(error.location.column, 7u);
    ASSERT_TRUE(error.message.find("unexpected end of file") != std::string::npos)
        << "got: " << error.message;
}

TEST(Error, UnclosedBracket) {
    std::string input = "arr = [1, 2, 3";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 1u);
    ASSERT_EQ(error.location.column, 15u);
    ASSERT_TRUE(error.message.find("]") != std::string::npos)
        << "got: " << error.message;
}

TEST(Error, InvalidValue) {
    std::string input = "key = !!!";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 1u);
    ASSERT_EQ(error.location.column, 7u);
    ASSERT_FALSE(error.message.empty());
}

TEST(Error, EmptyKey) {
    std::string input = "= 42";
    auto error = test::ParseExpectFail(input);
    ASSERT_FALSE(error.ok());
    ASSERT_EQ(error.location.line, 1u);
    ASSERT_EQ(error.location.column, 2u);
    ASSERT_TRUE(error.message.find("empty key") != std::string::npos)
        << "got: " << error.message;
}

TEST(Error, SuccessState) {
    // Successful parse should have ok() == true and empty message
    std::string valid = "key = 42";
    auto result = toml::parse_string(valid);
    ASSERT_TRUE(result.ok());
    ASSERT_TRUE(result.error.message.empty());
    ASSERT_EQ(result.error.location.line, 0u);
    ASSERT_EQ(result.error.location.column, 0u);
    ASSERT_TRUE(result.error.to_string().empty());
}

TEST(Error, ToStringFormat) {
    // to_string should produce "line N, column M: message" format
    std::string input = "key = !!!";
    auto error = test::ParseExpectFail(input);
    std::string str = error.to_string();
    ASSERT_TRUE(str.find("line 1") != std::string::npos);
    ASSERT_TRUE(str.find("column 7") != std::string::npos);
    ASSERT_FALSE(str.empty());
}

RUN_ALL_TESTS()
