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

#ifndef TOML_IMPL_NODE_IMPL_H_
#define TOML_IMPL_NODE_IMPL_H_

#include <atomic>
#include <map>
#include <string>
#include <vector>

#include "toml/impl/node.h"

namespace TOML {

class NodeImpl {
    friend class Node;

public:
    NodeImpl(/* args */);
    virtual ~NodeImpl();
    virtual Types Type() const;

private:
    void Ref();
    int32_t Unref();

    std::atomic<int32_t> ref_count_;
};

class String : public NodeImpl {
    friend class Node;

public:
    ~String();

    Types Type() const override;
    const std::string &Value() const;
    void SetValue(const std::string &value);
    void SetValue(const char *value);

private:
    String();
    String(const std::string &s);

    std::string value_;
};

class Boolean : public NodeImpl {
    friend class Node;

public:
    ~Boolean();

    Types Type() const override;
    bool Value() const;
    void SetValue(bool value);

private:
    Boolean();
    Boolean(bool b);

    bool value_;
};

class Integer : public NodeImpl {
    friend class Node;

public:
    template <bool SIGNED>
    struct RealType {
        using type = void;
    };

    ~Integer();

    Types Type() const override;

    void SetValue(int64_t value);
    void SetValue(uint64_t value);

    template <bool SIGNED = true>
    typename RealType<SIGNED>::type Value() {
        if (SIGNED) {
            return *reinterpret_cast<int64_t *>(value_);
        } else {
            return *reinterpret_cast<int64_t *>(value_);
        }
    }

private:
    Integer();
    Integer(int64_t v);
    Integer(uint64_t v);

    char value_[sizeof(int64_t)];
};

template <>
struct Integer::RealType<true> {
    using type = int64_t;
};
template <>
struct Integer::RealType<false> {
    using type = uint64_t;
};

class Float : public NodeImpl {
    friend class Node;

public:
    ~Float();

    double Value() const;
    void SetValue(double);
    Types Type() const override;

private:
    Float();
    Float(double d);

    double value_;
};

class DateTime : public NodeImpl {
    friend class Node;

public:
    struct Detail {
        struct {
            uint32_t tm_usec;                                /* Microseconds. [0-999999] */
            uint16_t tm_sec;                                 /* Seconds.	   [0-60] */
            uint8_t tm_min;                                  /* Minutes.	   [0-59] */
            uint8_t tm_hour;                                 /* Hours.	       [0-23] */
            uint8_t tm_day;                                  /* Day.		   [1-31] */
            uint8_t tm_mon;                                  /* Month.	       [0-11] */
            uint16_t tm_year;                                /* Year.	       [0-65535] */
            uint32_t tm_gmtoff; /* Seconds offset for UTC */ // %z +08:00 +0800
            char tm_zone[12]; /* TimeZone */                 //  %Z CST
        } buffer;
        uint32_t *gmtoff, *microsecond;
        uint16_t *year, *second;
        uint8_t *month, *day, *hour, *minute;
        char *zone;

        void SetYear(uint16_t y);
        void SetMonth(uint8_t m);
        void SetDay(uint8_t d);
        void SetHour(uint8_t h);
        void SetMinute(uint8_t m);
        void SetSecond(uint16_t s);
        void SetMicroSecond(uint32_t us);
        void SetUTCOffset(uint32_t off);
        void SetTimeZone(const char *z);
    };

    ~DateTime();

    const std::string &RawString() const;
    void SetRawString(const std::string &);
    const DateTime::Detail &Value() const;
    void SetValue(DateTime::Detail *detail);
    Types Type() const override;

private:
    DateTime();
    DateTime(const std::string &s);

    std::string raw_;

    DateTime::Detail value_;
};

class Object : public NodeImpl {
    friend class Node;

public:
    using RealType = std::map<std::string, Node>;
    using Iterator = RealType::iterator;

    ~Object();

    Types Type() const override;
    void Insert(const std::string &key, const Node &value);
    void Replace(const std::string &key, const Node &value);

    inline Iterator begin() { return obj_.begin(); }
    inline Iterator end() { return obj_.end(); }

    Node &operator[](std::string &&key);
    Node &operator[](const std::string &key);
    Node &Get(const std::string &key);
    Node &Get(std::string &&key);

private:
    Object();

    Object::RealType obj_;
};

class Array : public NodeImpl {
    friend class Node;

public:
    using RealType = std::vector<Node>;
    using Iterator = RealType::iterator;

    ~Array();

    Types Type() const override;
    void Insert(size_t index, const Node &value);
    void PushBack(const Node &value);

    inline Iterator begin() { return array_.begin(); }
    inline Iterator end() { return array_.end(); }

    const Node &operator[](size_t pos) const;
    Node &operator[](size_t pos);
    const Node &At(size_t pos) const;
    Node &At(size_t pos);

private:
    Array();

    Array::RealType array_;
};

} // namespace TOML

#endif // TOML_IMPL_NODE_IMPL_H_
