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
#include <assert.h>
#include <string.h>
#include <iostream>
#include "src/common.h"

namespace TOML {
namespace internal {
bool Reader::ParseComplexKey() {

    std::vector<std::string> path;

    bool find_a_separator = true;

    bool result = false;
    std::string key;

    while (remaining_input_ > 0) {
        uint8_t c = *input_;
        switch (c) {
        case '\'':
        case '\"':
            input_ += 1;
            remaining_input_ -= 1;
            if (c == '\'') {
                if (!GetLiteralString()) {
                    goto __exit;
                }
            } else {
                if (!GetBasicString()) {
                    goto __exit;
                }
            }
            find_a_separator = false;
            continue;
        case ' ':
        case '\t':
            break;
        case '.':
            path.push_back(strings_);
            key.append(strings_);
            key.push_back('.');
            strings_.clear();
            find_a_separator = true;
            break;
        case '=':
            if (!find_a_separator) {
                path.push_back(strings_);
                key.append(strings_);
                strings_.clear();
                result = true;
            }
            goto __exit;
        default:
            if (!IsValidCharForRawKey(c)) {
                goto __exit;
            }
            if (find_a_separator) {
                find_a_separator = false;
            }
            StringAddChar(c);
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:

    if (!result) path.clear();
    std::swap(current_.key_path, path);
    std::swap(current_.key, key);
    return result;
}

bool Reader::UsingComplexKey() {
    /*
        表不能定义多于一次，不允许使用 [table] 头重定义这样的表，
        同样地，使用点分隔键来重定义已经以 [table] 形式定义过的
        表也是不允许的。
    */
    std::string prefix;
    if (current_.key_path.size() > 1) {
        if (current_.title_path.empty()) {
            prefix = ComplexPathPrefix(current_.key_path);
        } else {
            prefix = current_.title;
            prefix.push_back('.');
            prefix.append(ComplexPathPrefix(current_.key_path));
        }
        auto it = table_map_.find(prefix);
        if (it == table_map_.end()) {
            table_map_[prefix] = ALLOWED_REPEATED;
        } else if (it->second == DISABLE_REPEATED) {
            desc_ = "node \"" + prefix + "\" redefine";
            return false;
        }
    }

    // 点分隔键为最后一个键名前的每个键名创建并定义一个表，
    // 倘若这些表尚未被创建的话
    complex_key_depth_ = 0;

    Node parent = stack_.top();
    int count   = static_cast<int>(current_.key_path.size());
    for (int i = 0; i < count - 1; i++) {
        assert(parent.Type() == Types::TOML_TABLE);
        auto key = current_.key_path[i];
        Node obj = parent.As<kTable>()->Get(key);
        if (obj) {
            if (obj.Type() != Types::TOML_TABLE) {
                desc_ = "\"" + key + "\" already exists and it is not an table";
                return false;
            }
            if (obj.As<kTable>()->Inlined()) {
                desc_ = "\"" + key + "\" is an inline table and cannot be modified";
                return false;
            }
        } else {
            obj = Node::CreateTable();
            parent.As<kTable>()->Insert(key, obj);
        }
        complex_key_depth_++;
        PushStack(obj);
        parent = obj;
    }

    auto key = GetVectorLastElement(current_.key_path);
    if (parent.Type() != Types::TOML_TABLE) {
        desc_ = "internal error (the parent of \"" + key + "\" is not an table)";
        return false;
    }
    if (parent.As<kTable>()->Exists(key)) {
        desc_ = "\"" + key + "\" already exists";
        return false;
    }
    std::swap(key_, key);
    return true;
}

} // namespace internal
} // namespace TOML
