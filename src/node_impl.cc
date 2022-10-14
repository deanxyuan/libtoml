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

#include "toml/impl/node_impl.h"
#include <string.h>
#include <sstream>

namespace TOML {

NodeImpl::NodeImpl(/* args */)
    : ref_count_(0) {}
NodeImpl::~NodeImpl() {}
Types NodeImpl::Type() const { return Types::TOML_NULL; }
void NodeImpl::Ref() { ref_count_.fetch_add(1, std::memory_order_relaxed); }
int32_t NodeImpl::Unref() { return ref_count_.fetch_sub(1, std::memory_order_acq_rel); }
std::string NodeImpl::ToString() const { return std::string("null"); }
std::string NodeImpl::ToJSON() const { return std::string("null"); }

// ------------------------------
static std::string StringToString(const std::string &value) {
    std::string ss;
    ss.push_back('\"');
    for (auto c : value) {
        if (c == '\"') {
            ss.push_back('\\');
        }
        ss.push_back(c);
    }
    ss.push_back('\"');
    return ss;
}

String::String()
    : NodeImpl() {}
String::String(const std::string &s)
    : NodeImpl()
    , value_(s) {}
String::~String() {}
const std::string &String::Value() const { return value_; }
void String::SetValue(const std::string &value) { value_ = value; }
void String::SetValue(const char *value) { value_ = std::string(value); }
Types String::Type() const { return Types::TOML_STRING; }
std::string String::ToString() const { return StringToString(value_); }
std::string String::ToJSON() const { return StringToString(value_); }

// ------------------------------
Boolean::Boolean()
    : NodeImpl()
    , value_(false) {}
Boolean::Boolean(bool b)
    : NodeImpl()
    , value_(b) {}
Boolean::~Boolean() {}
bool Boolean::Value() const { return value_; }
void Boolean::SetValue(bool value) { value_ = value; }
Types Boolean::Type() const { return Types::TOML_BOOLEAN; }
std::string Boolean::ToString() const { return value_ ? "true" : "false"; }
std::string Boolean::ToJSON() const { return value_ ? "true" : "false"; }

// ------------------------------
Integer::Integer()
    : NodeImpl() {
    memset(value_, 0, sizeof(value_));
}
Integer::Integer(int64_t v)
    : NodeImpl() {
    *reinterpret_cast<int64_t *>(value_) = v;
}
Integer::Integer(uint64_t v)
    : NodeImpl() {
    *reinterpret_cast<uint64_t *>(value_) = v;
}
Integer::~Integer() {}

void Integer::SetValue(int64_t value) { *reinterpret_cast<int64_t *>(value_) = value; }
void Integer::SetValue(uint64_t value) { *reinterpret_cast<uint64_t *>(value_) = value; }
Types Integer::Type() const { return Types::TOML_INTEGER; }
std::string Integer::ToString() const { return std::to_string(Value<true>()); }
std::string Integer::ToJSON() const { return std::to_string(Value<true>()); }

// ------------------------------
Float::Float()
    : NodeImpl()
    , value_(0) {}
Float::Float(double d)
    : NodeImpl()
    , value_(d) {}
Float::~Float() {}
double Float::Value() const { return value_; }
void Float::SetValue(double value) { value_ = value; }
Types Float::Type() const { return Types::TOML_FLOAT; }
std::string Float::ToString() const { return std::to_string(value_); }
std::string Float::ToJSON() const { return std::to_string(value_); }

// ------------------------------

DateTime::DateTime()
    : NodeImpl()
    , raw_() {
    memset(&value_, 0, sizeof(value_));
}
DateTime::DateTime(const std::string &s)
    : NodeImpl()
    , raw_(s) {
    memset(&value_, 0, sizeof(value_));
}

DateTime::~DateTime() {}
const std::string &DateTime::RawString() const { return raw_; }
void DateTime::SetRawString(const std::string &s) { raw_ = s; }
const DateTime::Detail &DateTime::Value() const { return value_; }
void DateTime::SetValue(DateTime::Detail *value) { value_ = *value; }
Types DateTime::Type() const { return Types::TOML_DATETIME; }
std::string DateTime::ToString() const { return RawString(); }
std::string DateTime::ToJSON() const { return StringToString(raw_); }
void DateTime::InitDetail(DateTime::Detail *dt) { dt->Reset(); }
DateTime::Detail::Detail() { memset(&data_, 0, sizeof(data_)); }
DateTime::Detail::Detail(const DateTime::Detail &oth) {
    memset(&data_, 0, sizeof(data_));
    memcpy(&data_.buffer, &oth.data_.buffer, sizeof(data_.buffer));
    if (oth.data_.gmtoff) {
        data_.gmtoff = &data_.buffer.tm_gmtoff;
    }
    if (oth.data_.microsecond) {
        data_.microsecond = &data_.buffer.tm_usec;
    }
    if (oth.data_.year) {
        data_.year = &data_.buffer.tm_year;
    }
    if (oth.data_.month) {
        data_.month = &data_.buffer.tm_mon;
    }
    if (oth.data_.day) {
        data_.day = &data_.buffer.tm_day;
    }
    if (oth.data_.hour) {
        data_.hour = &data_.buffer.tm_hour;
    }
    if (oth.data_.minute) {
        data_.minute = &data_.buffer.tm_min;
    }
    if (oth.data_.second) {
        data_.second = &data_.buffer.tm_sec;
    }
}
DateTime::Detail &DateTime::Detail::operator=(const DateTime::Detail &oth) {
    if (this != &oth) {
        memset(&data_, 0, sizeof(data_));
        memcpy(&data_.buffer, &oth.data_.buffer, sizeof(data_.buffer));
        if (oth.data_.gmtoff) {
            data_.gmtoff = &data_.buffer.tm_gmtoff;
        }
        if (oth.data_.microsecond) {
            data_.microsecond = &data_.buffer.tm_usec;
        }
        if (oth.data_.year) {
            data_.year = &data_.buffer.tm_year;
        }
        if (oth.data_.month) {
            data_.month = &data_.buffer.tm_mon;
        }
        if (oth.data_.day) {
            data_.day = &data_.buffer.tm_day;
        }
        if (oth.data_.hour) {
            data_.hour = &data_.buffer.tm_hour;
        }
        if (oth.data_.minute) {
            data_.minute = &data_.buffer.tm_min;
        }
        if (oth.data_.second) {
            data_.second = &data_.buffer.tm_sec;
        }
    }
    return *this;
}

void DateTime::Detail::SetYear(uint16_t y) {
    data_.buffer.tm_year = y;
    // set pointer
    data_.year = &data_.buffer.tm_year;
}
void DateTime::Detail::SetMonth(uint8_t m) {
    data_.buffer.tm_mon = m;
    // set pointer
    data_.month = &data_.buffer.tm_mon;
}
void DateTime::Detail::SetDay(uint8_t d) {
    data_.buffer.tm_day = d;
    // set pointer
    data_.day = &data_.buffer.tm_day;
}
void DateTime::Detail::SetHour(uint8_t h) {
    data_.buffer.tm_hour = h;
    // set pointer
    data_.hour = &data_.buffer.tm_hour;
}

void DateTime::Detail::SetMinute(uint8_t m) {
    data_.buffer.tm_min = m;
    // set pointer
    data_.minute = &data_.buffer.tm_min;
}

void DateTime::Detail::SetSecond(uint8_t s) {
    data_.buffer.tm_sec = s;
    // set pointer
    data_.second = &data_.buffer.tm_sec;
}
void DateTime::Detail::SetMicroSecond(uint32_t us) {
    data_.buffer.tm_usec = us;
    // set pointer
    data_.microsecond = &data_.buffer.tm_usec;
}
void DateTime::Detail::SetGMTOffset(uint8_t h, uint8_t m) {
    data_.buffer.tm_gmtoff = static_cast<int>(h) * 3600 + static_cast<int>(m) * 60;
    // set pointer
    data_.gmtoff = &data_.buffer.tm_gmtoff;
}

void DateTime::Detail::SetGMTOffset(uint32_t off) {
    data_.buffer.tm_gmtoff = off;
    // set pointer
    data_.gmtoff = &data_.buffer.tm_gmtoff;
}
void DateTime::Detail::SetSpecific(bool b) { data_.buffer.tm_specific = b ? 1 : 0; }
int DateTime::Detail::Year() { return data_.year ? *data_.year : -1; }
int DateTime::Detail::Month() { return data_.month ? *data_.month : -1; }
int DateTime::Detail::Day() { return data_.day ? *data_.day : -1; }
int DateTime::Detail::Hour() { return data_.hour ? *data_.hour : -1; }
int DateTime::Detail::Minute() { return data_.minute ? *data_.minute : -1; }
int DateTime::Detail::Second() { return data_.second ? *data_.second : -1; }
int DateTime::Detail::MicroSecond() { return data_.microsecond ? *data_.microsecond : -1; }
int DateTime::Detail::GMTOffset() { return data_.gmtoff ? *data_.gmtoff : -1; }
bool DateTime::Detail::Specific() { return data_.buffer.tm_specific != 0; }
void DateTime::Detail::Reset() { memset(&data_, 0, sizeof(data_)); }
// ------------------------------

Object::Object()
    : NodeImpl()
    , inlined_(false) {}

Object::~Object() {}

Types Object::Type() const { return Types::TOML_OBJECT; }
bool Object::Insert(const std::string &key, const Node &value) {
    if (inlined_) return false;
    obj_[key] = value;
    return true;
}
Node &Object::operator[](std::string &&key) { return obj_[key]; }
Node &Object::operator[](const std::string &key) { return obj_[key]; }
Node Object::Get(const std::string &key) {
    auto it = obj_.find(key);
    if (it != obj_.end()) {
        return it->second;
    }
    return Node();
}

Node Object::Get(std::string &&key) {
    auto it = obj_.find(key);
    if (it != obj_.end()) {
        return it->second;
    }
    return Node();
}

std::string Object::ToStringImpl(char key_delimiter, char value_delimiter) const {
    if (obj_.empty()) return std::string("{}");

    std::stringstream ss;
    ss << "{";

    auto it = obj_.begin();

    int i = 0;
    for (; i < static_cast<int>(obj_.size()) - 1; i++) {
        ss << StringToString(it->first);
        ss << key_delimiter;
        ss << it->second.ToString();
        ss << value_delimiter;
        it++;
    }
    ss << StringToString(it->first);
    ss << key_delimiter;
    ss << it->second.ToString() << "}";
    return ss.str();
}
std::string Object::ToString() const { return ToStringImpl('=', ','); }
std::string Object::ToJSON() const { return ToStringImpl(':', ','); }
bool Object::Exists(const std::string &key) const {
    auto it = obj_.find(key);
    return (it != obj_.end());
}
bool Object::ExistsWithType(const std::string &key, Types type) const {
    auto it = obj_.find(key);
    return (it != obj_.end() && it->second.Type() == type);
}

// ------------------------------
Array::Array()
    : NodeImpl()
    , static_(false) {}
Array::~Array() {}

Types Array::Type() const { return Types::TOML_ARRAY; }
void Array::Insert(size_t index, const Node &value) {
    if (array_.empty() || index >= array_.size()) {
        array_.push_back(value);
        return;
    }

    auto iter = array_.begin();
    std::advance(iter, index);
    array_.insert(iter, value);
}

void Array::PushBack(const Node &value) { array_.push_back(value); }
const Node &Array::operator[](size_t pos) const { return array_[pos]; }
Node &Array::operator[](size_t pos) { return array_[pos]; }
const Node &Array::At(size_t pos) const { return array_[pos]; }
Node &Array::At(size_t pos) { return array_[pos]; }
std::string Array::ToString() const {
    if (array_.empty()) return std::string("[]");

    std::stringstream ss;
    ss << '[';

    int i = 0;
    for (; i < static_cast<int>(array_.size()) - 1; i++) {
        ss << array_[i].ToString();
        ss << ',';
    }
    ss << array_[i].ToString() << ']';
    return ss.str();
}
std::string Array::ToJSON() const { return ToString(); }
} // namespace TOML
