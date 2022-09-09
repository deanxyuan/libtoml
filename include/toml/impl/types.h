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

#ifndef TOML_IMPL_TYPES_H_
#define TOML_IMPL_TYPES_H_

#include <stdint.h>
#include <atomic>
#include <map>
#include <string>
#include <vector>

namespace TOML {

enum Type {
    TOML_NULL,
    TOML_BOOL,
    TOML_NUMBER,
    TOML_DOUBLE,
    TOML_STRING,
    TOML_TIME, // Time or Date or DateTime
    TOML_OBJECT,
    TOML_ARRAY,
};

class Field {

public:
    Field(/* args */);
    virtual ~Field();

    void set_key(const std::string &k);
    const std::string &key() const;

    virtual intptr_t value() const;
    virtual void set_value(intptr_t);
    virtual Type type() const;

private:
    std::string key_;
};

class StringField : public Field {

public:
    StringField() {}
    ~StringField() {}

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

private:
    std::string value_;
};

class BooleanField : public Field {
public:
    BooleanField()
        : value_(false) {}

    ~BooleanField() {}

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

private:
    bool value_;
};

class IntegerField : public Field {
public:
    union Number {
        int64_t i64;
        uint64_t u64;
        //
        // buff[0-7] : for 64 bit integer value,
        // buff[8] : 0 signed, 1 unsigned,
        // buff[9] : reserved
        //
        char buff[10];
    };

    IntegerField();
    ~IntegerField();

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

private:
    Number value_;
};

class DoubleField : public Field {
public:
    DoubleField()  = default;
    ~DoubleField() = default;

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

private:
    double value_;
};

class ObjectField : public Field {
public:
    using Object = std::map<std::string, std::string>;

    ObjectField()  = default;
    ~ObjectField() = default;

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

    inline bool operator==(const ObjectField &rhs) const { return rhs.value_ == value_; }
    inline bool operator!=(const ObjectField &rhs) const { return rhs.value_ != value_; }

private:
    Object value_;
};

class ArrayField : public Field {
public:
    using Array = std::vector<std::string>;

    ArrayField()  = default;
    ~ArrayField() = default;

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

    inline bool operator==(const ArrayField &rhs) const { return rhs.value_ == value_; }
    inline bool operator!=(const ArrayField &rhs) const { return rhs.value_ != value_; }

private:
    Array value_;
};

class TimeField : public Field {
public:
    struct Time {
        int32_t tm_usec;   /* Microseconds. [0-999999] */
        uint8_t tm_sec;    /* Seconds.	   [0-60] */
        uint8_t tm_min;    /* Minutes.	   [0-59] */
        uint8_t tm_hour;   /* Hours.	       [0-23] */
        uint8_t tm_mday;   /* Day.		   [1-31] */
        uint8_t tm_mon;    /* Month.	       [0-11] */
        uint16_t tm_year;  /* Year.	       [0-65535] */
        int32_t tm_gmtoff; /* UTC %z +08:00 +0800*/
        char tm_zone[12];  /* TimeZone %Z CST */
    };

    TimeField();
    ~TimeField();

    intptr_t value() const override;
    void set_value(intptr_t) override;
    Type type() const override;

private:
    Time value_;
};

} // namespace TOML

#endif // TOML_IMPL_TYPES_H_
