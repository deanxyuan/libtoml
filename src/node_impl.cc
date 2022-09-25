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

// ------------------------------

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
std::string String::ToString() const {
    std::stringstream ss;
    ss << "\"" << value_ << "\"";
    return ss.str();
}

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

void DateTime::InitDetail(DateTime::Detail *detail) { memset(detail, 0, sizeof(*detail)); }
void DateTime::Detail::SetYear(uint16_t y) {
    buffer.tm_year = y;
    year           = &buffer.tm_year;
}
void DateTime::Detail::SetMonth(uint8_t m) {
    buffer.tm_mon = m;
    month         = &buffer.tm_mon;
}
void DateTime::Detail::SetDay(uint8_t d) {
    buffer.tm_day = d;
    day           = &buffer.tm_day;
}
void DateTime::Detail::SetHour(uint8_t h) {
    buffer.tm_hour = h;
    hour           = &buffer.tm_hour;
}

void DateTime::Detail::SetMinute(uint8_t m) {
    buffer.tm_min = m;
    minute        = &buffer.tm_min;
}

void DateTime::Detail::SetSecond(uint16_t s) {
    buffer.tm_sec = s;
    second        = &buffer.tm_sec;
}
void DateTime::Detail::SetMicroSecond(uint32_t us) {
    buffer.tm_usec = us;
    microsecond    = &buffer.tm_usec;
}
void DateTime::Detail::SetGMTOffset(uint8_t h, uint8_t m) {
    buffer.tm_gmtoff = static_cast<int>(h) * 3600 + static_cast<int>(m) * 60;
    gmtoff           = &buffer.tm_gmtoff;
}
void DateTime::Detail::SetGMTOffset(uint32_t off) {
    buffer.tm_gmtoff = off;
    gmtoff           = &buffer.tm_gmtoff;
}
void DateTime::Detail::SetTimeZone(const char *z) {
    size_t len = strlen(z);
    if (len == 0) {
        return;
    }
    if (len > sizeof(buffer.tm_zone) - 1) {
        len = sizeof(buffer.tm_zone) - 1;
    }
    memcpy(buffer.tm_zone, z, len);
    buffer.tm_zone[len] = '\0';
    zone                = &buffer.tm_zone[0];
}
// ------------------------------

Object::Object()
    : NodeImpl() {}

Object::~Object() {}

Types Object::Type() const { return Types::TOML_OBJECT; }
void Object::Insert(const std::string &key, const Node &value) { obj_[key] = value; }
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

std::string Object::ToString() const {
    if (obj_.empty()) return std::string("{}");

    std::stringstream ss;
    ss << "{";

    auto it = obj_.begin();

    int i = 0;
    for (; i < static_cast<int>(obj_.size()) - 1; i++) {
        ss << "\"" << it->first << "\" = ";
        ss << it->second.ToString() << ",";
        it++;
    }
    ss << "\"" << it->first << "\" = ";
    ss << it->second.ToString() << "}";
    return ss.str();
}
bool Object::Exists(const std::string &key) const {
    auto it = obj_.find(key);
    return (it != obj_.end());
}

// ------------------------------
Array::Array()
    : NodeImpl() {}
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
    ss << "[";

    int i = 0;
    for (; i < static_cast<int>(array_.size()) - 1; i++) {
        ss << array_[i].ToString() << ",";
    }
    ss << array_[i].ToString() << "]";
    return ss.str();
}

} // namespace TOML
