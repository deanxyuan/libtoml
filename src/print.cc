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

void Reader::UpdateNode(const Node &node) {
    Node &parent = stack_.top();
    if (parent.Type() == Types::TOML_ARRAY) {
        std::cout << "Value[" << node.TypeString() << "]:" << node.ToString() << std::endl;
    } else {
        std::cout << "Key:" << key_ << ", Value[" << node.TypeString() << "]:" << node.ToString()
                  << std::endl;
    }
    UpdateNodeImpl(node);
}
void Reader::PushStack(const Node &node) {
    std::cout << "Push Stack:" << node.TypeString() << std::endl;
    PushStackImpl(node);
}
bool Reader::PopStack(Types type) {
    if (PopStackImpl(type)) {
        std::cout << "Pop Stack:" << TypeString(type) << std::endl;
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
void Reader::PrintNode(const Node &node) { std::cout << "Node:" << node.ToString() << std::endl; }
} // namespace internal
} // namespace TOML
