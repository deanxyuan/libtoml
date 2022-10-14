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

#include "src/reader.h"
#include <string.h>
#include <iostream>
#include "src/common.h"

namespace TOML {
namespace internal {

bool Reader::StaticArrayPop() {
    Node &parent = stack_.top();
    if (parent.Type() == Types::TOML_ARRAY) {
        parent.As<kArray>()->SetStatic();
        stack_.pop();
        return true;
    }
    return false;
}
bool Reader::InlinedTablePop() {
    Node &parent = stack_.top();
    if (parent.Type() == Types::TOML_OBJECT) {
        parent.As<kObject>()->SetInlined();
        stack_.pop();
        return true;
    }
    return false;
}
void Reader::UpdateNode(const Node &node) {
    Node &parent = stack_.top();
    if (parent.Type() == Types::TOML_ARRAY) {
        parent.As<kArray>()->PushBack(node);
    } else {
        parent.As<kObject>()->Insert(key_, node);
    }
    key_.clear();
    strings_.clear();
}
void Reader::PushStack(const Node &node) { stack_.push(node); }
bool Reader::PopStack(Types type) {
    if (stack_.size() == 1) {
        return false;
    }
    Node &parent = stack_.top();
    if (parent.Type() == type) {
        stack_.pop();
        return true;
    }
    return false;
}
int Reader::StackDepth() { return static_cast<int>(stack_.size()); }
bool Reader::RestoreStack(int depth) {
    while (depth > 0) {
        if (!PopStack(Types::TOML_OBJECT)) {
            return false;
        }
        depth--;
    }
    return true;
}
bool Reader::ForceRestoreStack(int depth) {
    while (depth > 0 && !stack_.empty()) {
        stack_.pop();
        depth--;
    }
    return (depth == 0);
}
Node Reader::PushEmptyObject() {
    auto node = Node::CreateObject();
    UpdateNode(node);
    PushStack(node);
    return node;
}
Node Reader::PushEmptyArray() {
    auto node = Node::CreateArray();
    UpdateNode(node);
    PushStack(node);
    return node;
}
} // namespace internal
} // namespace TOML
