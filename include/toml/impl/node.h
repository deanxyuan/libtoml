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

#ifndef TOML_IMPL_NODE_H_
#define TOML_IMPL_NODE_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "toml/impl/types.h"

namespace TOML {

class NodeImpl;

class Boolean;
class String;
class Integer;
class Float;
class DateTime;
class Object;
class Array;

template <Types T>
struct ToNodeImpl {
    using type = void;
};
template <>
struct ToNodeImpl<Types::TOML_BOOLEAN> {
    using type = TOML::Boolean;
};
template <>
struct ToNodeImpl<Types::TOML_STRING> {
    using type = TOML::String;
};
template <>
struct ToNodeImpl<Types::TOML_INTEGER> {
    using type = TOML::Integer;
};
template <>
struct ToNodeImpl<Types::TOML_FLOAT> {
    using type = TOML::Float;
};
template <>
struct ToNodeImpl<Types::TOML_DATETIME> {
    using type = TOML::DateTime;
};
template <>
struct ToNodeImpl<Types::TOML_OBJECT> {
    using type = TOML::Object;
};
template <>
struct ToNodeImpl<Types::TOML_ARRAY> {
    using type = TOML::Array;
};

std::string TypeString(Types type);

class Node final {

public:
    static Node CreateNode(Types type);
    static Node CreateBoolean(bool b);
    static Node CreateString(const std::string &s);
    static Node CreateInteger(int64_t n);
    static Node CreateInteger(uint64_t n);
    static Node CreateFloat(double d);
    static Node CreateDateTime(const std::string &s);
    static Node CreateObject();
    static Node CreateArray();

    Node();
    ~Node();

    Node(const Node &oth);
    Node &operator=(const Node &oth);

    Node(Node &&oth);
    Node &operator=(Node &&oth);

    Types Type() const;
    std::string TypeString() const;

    operator bool() const { return impl_ != nullptr; }

    template <Types T>
    typename ToNodeImpl<T>::type *As() {
        return dynamic_cast<typename ToNodeImpl<T>::type *>(impl_);
    }

    // Use as a table
    TOML::Object *AsRoot();
    TOML::Object *AsTable();
    TOML::Array *AsArray();

    std::string ToString() const;
    std::string ToJSON() const;

private:
    Node(NodeImpl *impl)
        : impl_(impl) {}

    void Release();

    NodeImpl *impl_;
};

} // namespace TOML

#endif // TOML_IMPL_NODE_H_
