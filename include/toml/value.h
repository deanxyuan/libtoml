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

#ifndef TOML_VALUE_H_
#define TOML_VALUE_H_

#include "toml/types.h"
#include "toml/datetime.h"
#include "toml/array.h"
#include "toml/table.h"
#include <string>
#include <stdexcept>
#include <type_traits>

namespace TOML {

class Value {
public:
    // 构造函数
    Value();                          // null
    Value(bool v);
    Value(int v);                     // kInteger (消除 int 到 int64_t 的歧义)
    Value(int64_t v);                 // kInteger
    Value(unsigned int v);            // kUInteger (消除 unsigned int 到 uint64_t 的歧义)
    Value(uint64_t v);                // kUInteger
    Value(double v);
    Value(const char* v);
    Value(std::string v);
    Value(DateTime v);
    Value(Array v);
    Value(Table v);

    // 拷贝和移动
    Value(const Value& other);
    Value(Value&& other) noexcept;
    Value& operator=(const Value& other);
    Value& operator=(Value&& other) noexcept;

    ~Value();

    // 类型查询
    Type type() const { return type_; }
    bool is_null() const { return type_ == kNull; }
    bool is_boolean() const { return type_ == kBoolean; }
    bool is_integer() const {
        return type_ == kInteger || type_ == kUInteger;
    }
    bool is_float() const { return type_ == kFloat; }
    bool is_string() const { return type_ == kString; }
    bool is_datetime() const { return type_ == kDateTime; }
    bool is_table() const { return type_ == kTable; }
    bool is_array() const { return type_ == kArray; }

    // 类型安全访问（抛 bad_value_access on mismatch）
    bool as_bool() const;
    int64_t as_integer() const;
    uint64_t as_uinteger() const;
    double as_float() const;
    const std::string& as_string() const;
    const DateTime& as_datetime() const;
    const Table& as_table() const;
    const Array& as_array() const;

    // 可变访问
    Table& as_table_mut();
    Array& as_array_mut();

    // 序列化
    std::string to_toml() const;
    std::string to_json() const;

    // 显式布尔转换（用于条件判断）
    explicit operator bool() const { return !is_null(); }

private:
    void destroy();
    void copy_from(const Value& other);
    void move_from(Value&& other);

    Type type_;
    union Storage {
        bool bool_val;
        int64_t int_val;
        uint64_t uint_val;
        double float_val;
        std::string* string_val;
        DateTime datetime_val;
        Array* array_val;
        Table* table_val;

        Storage() {}
        ~Storage() {}
    } storage_;
};

// 类型检查异常
class bad_value_access : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace TOML

#endif // TOML_VALUE_H_
