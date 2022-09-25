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

#include "toml/impl/node.h"
#include "toml/impl/node_impl.h"

// ------------------------------
namespace TOML {

Node::Node()
    : impl_(nullptr) {}

Node::~Node() { Release(); }

Node::Node(const Node &oth) {
    if (oth.impl_) {
        oth.impl_->Ref();
    }
    impl_ = oth.impl_;
}
Node &Node::operator=(const Node &oth) {
    if (this != &oth) {
        if (oth.impl_) {
            oth.impl_->Ref();
        }
        Release();
        impl_ = oth.impl_;
    }
    return *this;
}

Node::Node(Node &&oth) {
    impl_     = oth.impl_;
    oth.impl_ = nullptr;
}

Node &Node::operator=(Node &&oth) {
    if (this != &oth) {
        Release();
        impl_     = oth.impl_;
        oth.impl_ = nullptr;
    }
    return *this;
}

void Node::Release() {
    if (!impl_) return;
    auto prev = impl_->Unref();
    if (prev == 1) {
        delete impl_;
        impl_ = nullptr;
    }
}

Types Node::Type() const {
    if (impl_) {
        return impl_->Type();
    }
    return Types::TOML_NULL;
}

std::string Node::ToString() const {
    if (impl_) {
        return impl_->ToString();
    }
    return std::string("null");
}

std::string Node::TypeString() const {
    if (impl_) {
        return TOML::TypeString(impl_->Type());
    }
    return std::string();
}

Node Node::CreateNode(Types type) {
    NodeImpl *impl = nullptr;
    switch (type) {
    case Types::TOML_BOOLEAN:
        impl = new Boolean();
        break;
    case Types::TOML_STRING:
        impl = new String();
        break;
    case Types::TOML_INTEGER:
        impl = new Integer();
        break;
    case Types::TOML_FLOAT:
        impl = new Float();
        break;
    case Types::TOML_DATETIME:
        impl = new DateTime();
        break;
    case Types::TOML_OBJECT:
        impl = new Object();
        break;
    case Types::TOML_ARRAY:
        impl = new Array();
        break;
    default:
        break;
    }
    if (impl) impl->Ref();
    return Node(impl);
}

Node Node::CreateBoolean(bool b) {
    auto impl = new Boolean(b);
    impl->Ref();
    return Node(impl);
}

Node Node::CreateString(const std::string &s) {
    auto impl = new String(s);
    impl->Ref();
    return Node(impl);
}

Node Node::CreateInteger(int64_t n) {
    auto impl = new Integer(n);
    impl->Ref();
    return Node(impl);
}

Node Node::CreateInteger(uint64_t n) {
    auto impl = new Integer(n);
    impl->Ref();
    return Node(impl);
}

Node Node::CreateFloat(double d) {
    auto impl = new Float(d);
    impl->Ref();
    return Node(impl);
}

Node Node::CreateDateTime(const std::string &s) {
    auto impl = new DateTime(s);
    impl->Ref();
    return Node(impl);
}
Node Node::CreateObject() {
    auto impl = new Object();
    impl->Ref();
    return Node(impl);
}

Node Node::CreateArray() {
    auto impl = new Array();
    impl->Ref();
    return Node(impl);
}

std::string TypeString(Types type) {
    std::string str;
    switch (type) {
    case TOML::Types::TOML_NULL:
        str = "Null";
        break;
    case TOML::Types::TOML_BOOLEAN:
        str = "Boolean";
        break;
    case TOML::Types::TOML_STRING:
        str = "String";
        break;
    case TOML::Types::TOML_INTEGER:
        str = "Integer";
        break;
    case TOML::Types::TOML_FLOAT:
        str = "Float";
        break;
    case TOML::Types::TOML_DATETIME:
        str = "Datetime";
        break;
    case TOML::Types::TOML_OBJECT:
        str = "Object";
        break;
    case TOML::Types::TOML_ARRAY:
        str = "Array";
        break;
    default:
        break;
    }
    return str;
}

} // namespace TOML
