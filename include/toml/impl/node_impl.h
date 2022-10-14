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
#include <unordered_map>
#include "toml/impl/node.h"

namespace TOML {

class NodeImpl {
    friend class Node;

public:
    NodeImpl(/* args */);
    virtual ~NodeImpl();
    virtual Types Type() const;
    virtual std::string ToString() const;
    virtual std::string ToJSON() const;

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
    std::string ToString() const override;
    std::string ToJSON() const override;

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
    std::string ToString() const override;
    std::string ToJSON() const override;

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
    typename RealType<SIGNED>::type Value() const {
        if (SIGNED) {
            return *reinterpret_cast<const int64_t *>(&value_[0]);
        } else {
            return *reinterpret_cast<const int64_t *>(&value_[0]);
        }
    }
    std::string ToString() const override;
    std::string ToJSON() const override;

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
    std::string ToString() const override;
    std::string ToJSON() const override;

private:
    Float();
    Float(double d);

    double value_;
};

class DateTime : public NodeImpl {
    friend class Node;

public:
    class Detail {
    public:
        Detail();
        Detail(const Detail &oth);
        Detail &operator=(const Detail &oth);

        void SetYear(uint16_t y);
        void SetMonth(uint8_t m);
        void SetDay(uint8_t d);
        void SetHour(uint8_t h);
        void SetMinute(uint8_t m);
        void SetSecond(uint8_t s);
        void SetMicroSecond(uint32_t us);
        void SetGMTOffset(uint8_t h, uint8_t m);
        void SetGMTOffset(uint32_t off);
        void SetSpecific(bool b);

        // If the data does not exist, return -1
        int Year();
        int Month();
        int Day();
        int Hour();
        int Minute();
        int Second();
        int MicroSecond();
        int GMTOffset();

        // Is a specific instant in time ?
        bool Specific();
        void Reset();

    private:
        struct {
            struct {
                uint16_t tm_year;    /* Year.	       [0-65535] */
                uint8_t tm_mon;      /* Month.	       [0-11] */
                uint8_t tm_day;      /* Day.		   [1-31] */
                uint8_t tm_hour;     /* Hours.	       [0-23] */
                uint8_t tm_min;      /* Minutes.	   [0-59] */
                uint8_t tm_sec;      /* Seconds.	   [0-60] */
                uint8_t tm_specific; /* 0: Local time, 1: Specific instant in time */
                uint32_t tm_usec;    /* Microseconds. [0-999999] */
                uint32_t tm_gmtoff;  /* Seconds offset of GMT */
            } buffer;
            uint32_t *gmtoff, *microsecond;
            uint16_t *year;
            uint8_t *month, *day, *hour, *minute, *second;
        } data_;
    };
    static void InitDetail(DateTime::Detail *dt);
    ~DateTime();

    const std::string &RawString() const;
    void SetRawString(const std::string &);
    const DateTime::Detail &Value() const;
    void SetValue(DateTime::Detail *detail);
    Types Type() const override;
    std::string ToString() const override;
    std::string ToJSON() const override;

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

    bool Insert(const std::string &key, const Node &value);

    inline Iterator begin() { return obj_.begin(); }
    inline Iterator end() { return obj_.end(); }
    inline bool Inlined() const { return inlined_; }
    inline void SetInlined() { inlined_ = true; }
    inline size_t size() const { return obj_.size(); }
    // If the key does not exist,
    // a empty node will be inserted
    // Just like Object[key]=Node();
    Node &operator[](std::string &&key);
    Node &operator[](const std::string &key);

    Node Get(const std::string &key);
    Node Get(std::string &&key);
    std::string ToString() const override;
    std::string ToJSON() const override;

    bool Exists(const std::string &key) const;

private:
    Object();
    std::string ToStringImpl(char key_delimiter, char value_delimiter) const;
    bool inlined_;
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
    inline bool Static() const { return static_; }
    inline void SetStatic() { static_ = true; }
    inline Iterator begin() { return array_.begin(); }
    inline Iterator end() { return array_.end(); }
    inline size_t size() const { return array_.size(); }

    const Node &operator[](size_t pos) const;
    Node &operator[](size_t pos);
    const Node &At(size_t pos) const;
    Node &At(size_t pos);
    std::string ToString() const override;
    std::string ToJSON() const override;

private:
    Array();
    bool static_;
    Array::RealType array_;
};

} // namespace TOML

#endif // TOML_IMPL_NODE_IMPL_H_
