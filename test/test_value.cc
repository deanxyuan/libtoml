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

TEST(Value, NullValue) {
    TOML::Value v;
    ASSERT_TRUE(v.is_null());
    ASSERT_EQ(v.type(), TOML::kNull);
    ASSERT_FALSE(v.is_boolean());
    ASSERT_FALSE(v.is_integer());
    ASSERT_FALSE(v.is_float());
    ASSERT_FALSE(v.is_string());
    ASSERT_FALSE(v.is_datetime());
    ASSERT_FALSE(v.is_array());
    ASSERT_FALSE(v.is_table());
    ASSERT_FALSE(static_cast<bool>(v));
}

TEST(Value, BooleanValue) {
    TOML::Value v_true(true);
    ASSERT_TRUE(v_true.is_boolean());
    ASSERT_EQ(v_true.type(), TOML::kBoolean);
    ASSERT_TRUE(v_true.as_bool());
    ASSERT_TRUE(static_cast<bool>(v_true));

    TOML::Value v_false(false);
    ASSERT_TRUE(v_false.is_boolean());
    ASSERT_FALSE(v_false.as_bool());
}

TEST(Value, IntegerValue) {
    TOML::Value v(42);
    ASSERT_TRUE(v.is_integer());
    ASSERT_EQ(v.type(), TOML::kInteger);
    ASSERT_EQ(v.as_integer(), 42);

    TOML::Value v_neg(-100);
    ASSERT_TRUE(v_neg.is_integer());
    ASSERT_EQ(v_neg.as_integer(), -100);

    TOML::Value v_zero(static_cast<int64_t>(0));
    ASSERT_TRUE(v_zero.is_integer());
    ASSERT_EQ(v_zero.as_integer(), 0);

    TOML::Value v_max(std::numeric_limits<int64_t>::max());
    ASSERT_TRUE(v_max.is_integer());
    ASSERT_EQ(v_max.as_integer(), std::numeric_limits<int64_t>::max());
}

TEST(Value, UIntegerValue) {
    TOML::Value v(static_cast<uint64_t>(123));
    ASSERT_TRUE(v.is_uinteger());
    ASSERT_FALSE(v.is_integer());
    ASSERT_EQ(v.type(), TOML::kUInteger);
    ASSERT_EQ(v.as_uinteger(), 123u);

    TOML::Value v_max(std::numeric_limits<uint64_t>::max());
    ASSERT_TRUE(v_max.is_uinteger());
    ASSERT_FALSE(v_max.is_integer());
    ASSERT_EQ(v_max.as_uinteger(), std::numeric_limits<uint64_t>::max());
}

TEST(Value, FloatValue) {
    TOML::Value v(3.14);
    ASSERT_TRUE(v.is_float());
    ASSERT_EQ(v.type(), TOML::kFloat);
    ASSERT_FLOAT_EQ(v.as_float(), 3.14);

    TOML::Value v_neg(-2.5);
    ASSERT_TRUE(v_neg.is_float());
    ASSERT_FLOAT_EQ(v_neg.as_float(), -2.5);

    TOML::Value v_zero(0.0);
    ASSERT_TRUE(v_zero.is_float());
    ASSERT_FLOAT_EQ(v_zero.as_float(), 0.0);
}

TEST(Value, StringValue) {
    TOML::Value v(std::string("hello"));
    ASSERT_TRUE(v.is_string());
    ASSERT_EQ(v.type(), TOML::kString);
    ASSERT_EQ(v.as_string(), "hello");

    TOML::Value v_cstr("world");
    ASSERT_TRUE(v_cstr.is_string());
    ASSERT_EQ(v_cstr.as_string(), "world");

    TOML::Value v_empty(std::string(""));
    ASSERT_TRUE(v_empty.is_string());
    ASSERT_EQ(v_empty.as_string(), "");
}

TEST(Value, DateTimeValue) {
    TOML::DateTime dt;
    dt.year = 2023;
    dt.month = 6;
    dt.day = 15;
    dt.hour = 10;
    dt.minute = 30;
    dt.second = 45;
    dt.present = TOML::DateTime::kYear | TOML::DateTime::kMonth |
                 TOML::DateTime::kDay | TOML::DateTime::kHour |
                 TOML::DateTime::kMinute | TOML::DateTime::kSecond;

    TOML::Value v(dt);
    ASSERT_TRUE(v.is_datetime());
    ASSERT_EQ(v.type(), TOML::kDateTime);
    const auto& result = v.as_datetime();
    ASSERT_EQ(result.year, 2023);
    ASSERT_EQ(result.month, 6);
    ASSERT_EQ(result.day, 15);
    ASSERT_EQ(result.hour, 10);
    ASSERT_EQ(result.minute, 30);
    ASSERT_EQ(result.second, 45);
}

TEST(Value, ArrayValue) {
    TOML::Array arr;
    arr.push_back(TOML::Value(1));
    arr.push_back(TOML::Value(2));
    arr.push_back(TOML::Value(3));

    TOML::Value v(std::move(arr));
    ASSERT_TRUE(v.is_array());
    ASSERT_EQ(v.type(), TOML::kArray);
    const auto& result = v.as_array();
    ASSERT_EQ(result.size(), 3u);
    ASSERT_EQ(result.at(0).as_integer(), 1);
    ASSERT_EQ(result.at(1).as_integer(), 2);
    ASSERT_EQ(result.at(2).as_integer(), 3);
}

TEST(Value, TableValue) {
    TOML::Table tbl;
    tbl.insert("name", TOML::Value(std::string("Alice")));
    tbl.insert("age", TOML::Value(30));

    TOML::Value v(std::move(tbl));
    ASSERT_TRUE(v.is_table());
    ASSERT_EQ(v.type(), TOML::kTable);
    const auto& result = v.as_table();
    ASSERT_EQ(result.size(), 2u);
    ASSERT_TRUE(test::CheckTableHasStringValue(result, "name", "Alice"));
    ASSERT_TRUE(test::CheckTableHasIntValue(result, "age", 30));
}

TEST(Value, TypeMismatchThrows) {
    TOML::Value int_val(42);
    ASSERT_THROW(int_val.as_bool(), TOML::bad_value_access);
    ASSERT_THROW(int_val.as_string(), TOML::bad_value_access);
    ASSERT_THROW(int_val.as_float(), TOML::bad_value_access);

    TOML::Value str_val(std::string("hello"));
    ASSERT_THROW(str_val.as_integer(), TOML::bad_value_access);
    ASSERT_THROW(str_val.as_bool(), TOML::bad_value_access);

    TOML::Value bool_val(true);
    ASSERT_THROW(bool_val.as_integer(), TOML::bad_value_access);
    ASSERT_THROW(bool_val.as_string(), TOML::bad_value_access);

    TOML::Value null_val;
    ASSERT_THROW(null_val.as_bool(), TOML::bad_value_access);
    ASSERT_THROW(null_val.as_integer(), TOML::bad_value_access);
}

TEST(Value, CopySemantics) {
    TOML::Value original(std::string("original"));
    TOML::Value copy(original);

    ASSERT_TRUE(copy.is_string());
    ASSERT_EQ(copy.as_string(), "original");

    // Modify copy, original should be unaffected
    TOML::Value modified(std::string("modified"));
    copy = modified;
    ASSERT_EQ(copy.as_string(), "modified");
    ASSERT_EQ(original.as_string(), "original");

    // Copy of array
    TOML::Array arr;
    arr.push_back(TOML::Value(10));
    TOML::Value arr_val(std::move(arr));
    TOML::Value arr_copy(arr_val);
    ASSERT_TRUE(arr_copy.is_array());
    ASSERT_EQ(arr_copy.as_array().size(), 1u);
    ASSERT_EQ(arr_copy.as_array().at(0).as_integer(), 10);
}

TEST(Value, MoveSemantics) {
    TOML::Value original(std::string("movable"));
    TOML::Value moved(std::move(original));

    ASSERT_TRUE(moved.is_string());
    ASSERT_EQ(moved.as_string(), "movable");

    // Move of array
    TOML::Array arr;
    arr.push_back(TOML::Value(42));
    TOML::Value arr_val(std::move(arr));
    TOML::Value arr_moved(std::move(arr_val));
    ASSERT_TRUE(arr_moved.is_array());
    ASSERT_EQ(arr_moved.as_array().size(), 1u);
    ASSERT_EQ(arr_moved.as_array().at(0).as_integer(), 42);
}

RUN_ALL_TESTS()
