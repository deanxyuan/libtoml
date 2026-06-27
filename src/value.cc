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

#include "toml/value.h"
#include "src/format_utils.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <sstream>

namespace TOML {

const char* type_name(Type type) {
    switch (type) {
    case kNull:      return "null";
    case kBoolean:   return "boolean";
    case kInteger:   return "integer";
    case kUInteger:  return "uinteger";
    case kFloat:     return "float";
    case kString:    return "string";
    case kDateTime:  return "datetime";
    case kTable:     return "table";
    case kArray:     return "array";
    default:         return "unknown";
    }
}

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

Value::Value() : type_(kNull) {}

Value::Value(bool v) : type_(kBoolean) {
    storage_.bool_val = v;
}

Value::Value(int v) : type_(kInteger) {
    storage_.int_val = v;
}

Value::Value(int64_t v) : type_(kInteger) {
    storage_.int_val = v;
}

Value::Value(uint64_t v) : type_(kUInteger) {
    storage_.uint_val = v;
}

Value::Value(unsigned int v) : type_(kUInteger) {
    storage_.uint_val = v;
}

Value::Value(double v) : type_(kFloat) {
    storage_.float_val = v;
}

Value::Value(const char* v) : type_(kString) {
    storage_.string_val = new std::string(v);
}

Value::Value(std::string v) : type_(kString) {
    storage_.string_val = new std::string(std::move(v));
}

Value::Value(DateTime v) : type_(kDateTime) {
    storage_.datetime_val = v;
}

Value::Value(Array v) : type_(kArray) {
    storage_.array_val = new Array(std::move(v));
}

Value::Value(Table v) : type_(kTable) {
    storage_.table_val = new Table(std::move(v));
}

// ---------------------------------------------------------------------------
// Copy constructor
// ---------------------------------------------------------------------------

Value::Value(const Value& other) : type_(kNull) {
    copy_from(other);
}

// ---------------------------------------------------------------------------
// Move constructor
// ---------------------------------------------------------------------------

Value::Value(Value&& other) noexcept : type_(kNull) {
    move_from(std::move(other));
}

// ---------------------------------------------------------------------------
// Copy assignment
// ---------------------------------------------------------------------------

Value& Value::operator=(const Value& other) {
    if (this != &other) {
        destroy();
        copy_from(other);
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Move assignment
// ---------------------------------------------------------------------------

Value& Value::operator=(Value&& other) noexcept {
    if (this != &other) {
        destroy();
        move_from(std::move(other));
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------

Value::~Value() {
    destroy();
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void Value::destroy() {
    switch (type_) {
    case kString:
        delete storage_.string_val;
        break;
    case kArray:
        delete storage_.array_val;
        break;
    case kTable:
        delete storage_.table_val;
        break;
    default:
        break;
    }
    type_ = kNull;
}

void Value::copy_from(const Value& other) {
    type_ = other.type_;
    switch (type_) {
    case kNull:      break;
    case kBoolean:   storage_.bool_val = other.storage_.bool_val; break;
    case kInteger:   storage_.int_val = other.storage_.int_val; break;
    case kUInteger:  storage_.uint_val = other.storage_.uint_val; break;
    case kFloat:     storage_.float_val = other.storage_.float_val; break;
    case kString:    storage_.string_val = new std::string(*other.storage_.string_val); break;
    case kDateTime:  storage_.datetime_val = other.storage_.datetime_val; break;
    case kArray:     storage_.array_val = new Array(*other.storage_.array_val); break;
    case kTable:     storage_.table_val = new Table(*other.storage_.table_val); break;
    }
}

void Value::move_from(Value&& other) {
    type_ = other.type_;
    switch (type_) {
    case kNull:      break;
    case kBoolean:   storage_.bool_val = other.storage_.bool_val; break;
    case kInteger:   storage_.int_val = other.storage_.int_val; break;
    case kUInteger:  storage_.uint_val = other.storage_.uint_val; break;
    case kFloat:     storage_.float_val = other.storage_.float_val; break;
    case kString:
        storage_.string_val = other.storage_.string_val;
        other.storage_.string_val = nullptr;
        break;
    case kDateTime:  storage_.datetime_val = other.storage_.datetime_val; break;
    case kArray:
        storage_.array_val = other.storage_.array_val;
        other.storage_.array_val = nullptr;
        break;
    case kTable:
        storage_.table_val = other.storage_.table_val;
        other.storage_.table_val = nullptr;
        break;
    }
    other.type_ = kNull;
}

// ---------------------------------------------------------------------------
// Type-safe accessors
// ---------------------------------------------------------------------------

bool Value::as_bool() const {
    if (type_ != kBoolean) {
        throw bad_value_access("Value is not a boolean");
    }
    return storage_.bool_val;
}

int64_t Value::as_integer() const {
    if (type_ != kInteger) {
        throw bad_value_access("Value is not an integer");
    }
    return storage_.int_val;
}

uint64_t Value::as_uinteger() const {
    if (type_ != kUInteger) {
        throw bad_value_access("Value is not an unsigned integer");
    }
    return storage_.uint_val;
}

double Value::as_float() const {
    if (type_ != kFloat) {
        throw bad_value_access("Value is not a float");
    }
    return storage_.float_val;
}

const std::string& Value::as_string() const {
    if (type_ != kString) {
        throw bad_value_access("Value is not a string");
    }
    return *storage_.string_val;
}

const DateTime& Value::as_datetime() const {
    if (type_ != kDateTime) {
        throw bad_value_access("Value is not a datetime");
    }
    return storage_.datetime_val;
}

const Table& Value::as_table() const {
    if (type_ != kTable) {
        throw bad_value_access("Value is not a table");
    }
    return *storage_.table_val;
}

const Array& Value::as_array() const {
    if (type_ != kArray) {
        throw bad_value_access("Value is not an array");
    }
    return *storage_.array_val;
}

Table& Value::as_table_mut() {
    if (type_ != kTable) {
        throw bad_value_access("Value is not a table");
    }
    return *storage_.table_val;
}

Array& Value::as_array_mut() {
    if (type_ != kArray) {
        throw bad_value_access("Value is not an array");
    }
    return *storage_.array_val;
}

// ---------------------------------------------------------------------------
// Serialization -- TOML
// ---------------------------------------------------------------------------

std::string Value::to_toml() const {
    switch (type_) {
    case kNull:
        return "";
    case kBoolean:
        return storage_.bool_val ? "true" : "false";
    case kInteger: {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%lld",
                       static_cast<long long>(storage_.int_val));
        return buf;
    }
    case kUInteger: {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%llu",
                       static_cast<unsigned long long>(storage_.uint_val));
        return buf;
    }
    case kFloat:
        return format_float_toml(storage_.float_val);
    case kString:
        return escape_toml_string(*storage_.string_val);
    case kDateTime:
        return storage_.datetime_val.to_toml();
    case kArray:
        return storage_.array_val->to_toml();
    case kTable:
        return storage_.table_val->to_toml();
    }
    return "";
}

// ---------------------------------------------------------------------------
// Serialization -- JSON
// ---------------------------------------------------------------------------

std::string Value::to_json() const {
    switch (type_) {
    case kNull:
        return "null";
    case kBoolean:
        return storage_.bool_val ? "true" : "false";
    case kInteger: {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%lld",
                       static_cast<long long>(storage_.int_val));
        return buf;
    }
    case kUInteger: {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%llu",
                       static_cast<unsigned long long>(storage_.uint_val));
        return buf;
    }
    case kFloat:
        return format_float_json(storage_.float_val);
    case kString:
        return json_escape_string(*storage_.string_val);
    case kDateTime:
        return json_escape_string(storage_.datetime_val.to_string());
    case kArray:
        return storage_.array_val->to_json();
    case kTable:
        return storage_.table_val->to_json();
    }
    return "null";
}

} // namespace TOML
