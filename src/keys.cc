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
            strings_.clear();
            find_a_separator = true;
            break;
        case '=':
            if (!find_a_separator) {
                path.push_back(strings_);
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

    if (result) {
        std::swap(path_, path);
        return true;
    }
    path_.clear();
    return false;
}

bool Reader::UsingComplexKey() {
    if (path_.empty()) return false;

    // 点分隔键为最后一个键名前的每个键名创建并定义一个表，
    // 倘若这些表尚未被创建的话
    complex_key_depth_ = 0;

    Node parent = stack_.top();
    int count   = static_cast<int>(path_.size());
    for (int i = 0; i < count - 1; i++) {
        assert(parent.Type() == Types::TOML_OBJECT);
        auto key = path_[i];
        Node obj = parent.As<kObject>()->Get(key);
        if (obj) {
            if (obj.Type() != Types::TOML_OBJECT) {
                desc_ = "\"" + key + "\" already exists and it is not an object";
                return false;
            }
            if (obj.As<kObject>()->Inlined()) {
                desc_ = "\"" + key + "\" is an inline table and cannot be modified";
                return false;
            }
        } else {
            obj = Node::CreateObject();
            parent.As<kObject>()->Insert(key, obj);
        }
        complex_key_depth_++;
        PushStack(obj);
        parent = obj;
    }

    auto key = path_[count - 1];
    if (!parent || parent.Type() != Types::TOML_OBJECT) {
        desc_ = "internal error";
        return false;
    }
    if (parent.As<kObject>()->Exists(key)) {
        desc_ = "\"" + key + "\" already exists";
        return false;
    }
    std::swap(key_, key);
    path_.clear();
    return true;
}

bool Reader::GetTitleOfTable() {
    if (StartsWith("[[")) {
        input_ += 2;
        remaining_input_ -= 2;
        if (GetTitleOfTableImpl() && StartsWith("]]")) {
            input_ += 2;
            remaining_input_ -= 2;
            is_table_title_ = false;
            return true;
        }
        return false;
    }
    input_++;
    remaining_input_--;
    if (GetTitleOfTableImpl() && StartsWith("]")) {
        input_++;
        remaining_input_--;
        is_table_title_ = true;
        return true;
    }
    return false;
}

bool Reader::GetTitleOfTableImpl() {
    bool find_a_separator = true;
    std::vector<std::string> path;

    bool result = false;
    raw_path_.clear();

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
            raw_path_.append(strings_);
            raw_path_.push_back('.');
            path.push_back(strings_);
            strings_.clear();
            find_a_separator = true;
            break;
        case ']':
            if (find_a_separator) {
                goto __exit;
            }
            raw_path_.append(strings_);
            path.push_back(strings_);
            strings_.clear();
            result = true;
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
    if (!result) path_.clear();
    std::swap(path_, path);
    return result;
}

bool Reader::UsingTableTitle() {
    return is_table_title_ ? UsingTableTitleImpl() : UsingArrayOfTableTitleImpl();
}

bool Reader::UsingTableTitleImpl() {
    if (path_.empty()) return false;

    table_depth_ = 0;

    if (!array_of_table_table_.empty() && path_.size() > 1) {
        auto it = array_of_table_table_.find(path_[0]);
        if (it != array_of_table_table_.end()) {
            // 说明是对表数组的引用
            Node node = it->second;
            int count = static_cast<int>(path_.size());
            for (int i = 1; i < count; i++) {
                if (!node || node.Type() != Types::TOML_OBJECT) {
                    desc_ = "internal error";
                    return false;
                }
                if (node.As<kObject>()->Exists(path_[i])) {
                    node = node.As<kObject>()->Get(path_[i]);
                } else {
                    Node obj = Node::CreateObject();
                    node.As<kObject>()->Insert(path_[i], obj);
                    node = obj;
                }
            }
            PushStack(node);
            table_depth_ = 1;
            return true;
        }
    }
    // 常规表头处理
    // 判断表头是否重复定义
    if (CheckTableTileRedefine()) {
        desc_ = "[" + raw_path_ + "] redefine";
        return false;
    }
    table_title_set_.insert(raw_path_);

    Node parent = stack_.top();
    int count   = static_cast<int>(path_.size());
    for (int i = 0; i < count - 1; i++) {
        auto key = path_[i];
        Node obj = parent.As<kObject>()->Get(key);
        if (!obj) {
            obj = Node::CreateObject();
            parent.As<kObject>()->Insert(key, obj);
        } else if (obj.Type() != Types::TOML_OBJECT) {
            desc_ = "\"" + key + "\" redefine";
            return false;
        } else if (obj.As<kObject>()->Inlined()) {
            desc_ = "\"" + key + "\" is an inline table and cannot be modified";
            return false;
        }
        parent = obj;
        PushStack(obj);
    }

    auto key = path_[path_.size() - 1];
    if (parent.As<kObject>()->Exists(key)) {
        desc_ = "\"" + key + "\" redefine";
        return false;
    }
    Node obj = Node::CreateObject();
    parent.As<kObject>()->Insert(key, obj);
    PushStack(obj);
    table_depth_ = count;
    return true;
}
bool Reader::UsingArrayOfTableTitleImpl() {
    if (path_.empty()) return false;
    if (path_.size() == 1) {
        // 表数组元素定义
        std::string key = path_[0];
        Node sub_array, parent = stack_.top();
        if (parent.Type() != Types::TOML_OBJECT) {
            desc_ = "internal error";
            return false;
        }
        if (!parent.As<kObject>()->Exists(key)) {
            // 首次出现
            sub_array = Node::CreateArray();
            parent.As<kObject>()->Insert(key, sub_array);
        } else {
            // 非首次出现，表示插入新的元素
            sub_array = parent.As<kObject>()->Get(key);
            if (sub_array.Type() != Types::TOML_ARRAY) {
                desc_ = "\"" + key + "\" already exists and it is not an array";
                return false;
            }
            if (!CheckArrayOfTableExists(key)) {
                desc_ = "\"" + key + "\" redefine";
                return false;
            }
        }
        // 创建一个空对象，入栈
        Node obj = Node::CreateObject();
        PushStackImpl(obj);
        sub_array.As<kArray>()->PushBack(obj);
        // 保存父节点(ARRAY)的最新元素
        array_of_table_table_[key] = obj;
        // 更新栈高度
        table_depth_ = 1;
        return true;
    }
    // 嵌套表数组，
    if (array_of_table_table_.empty()) {
        desc_ = "Missing definition of parent node \"" + path_[0] + "\"";
        return false;
    }
    // 路径是否有效
    // [[A.B.C]] prefix = A.B
    auto prefix = ComplexPathPrefix(raw_path_);

    // 路径是否存在
    auto it = array_of_table_table_.find(prefix);
    if (it == array_of_table_table_.end()) {
        desc_ = "Invalid path \"" + prefix + "\"";
        return false;
    }

    // 获取最新的节点名, 如 [[A.B.C]] 中的 C
    auto key = path_[path_.size() - 1];

    // 获取路径前缀的最后一次更新的 OBJECT
    Node sub_array, last_node = it->second;

    // 判断 C 数组是否存在
    if (last_node.As<kObject>()->Exists(key)) {
        sub_array = last_node.As<kObject>()->Get(key);
        if (sub_array.Type() != Types::TOML_ARRAY) {
            desc_ = "\"" + key + "\" must be array";
            return false;
        }

    } else {
        sub_array = Node::CreateArray();
        // 更新父节点，最后一次的 OBJECT
        last_node.As<kObject>()->Insert(key, sub_array);
    }
    // 创建一个空对象，入栈
    Node obj = Node::CreateObject();
    PushStackImpl(obj);
    sub_array.As<kArray>()->PushBack(obj);
    array_of_table_table_[raw_path_] = obj;

    // 更新栈高度
    table_depth_ = 1;
    return true;
}
bool Reader::CheckTableTileRedefine() {
    auto it = table_title_set_.find(raw_path_);
    return (it != table_title_set_.end());
}
bool Reader::CheckArrayOfTableExists(const std::string &name) {
    auto it = array_of_table_table_.find(name);
    return (it != array_of_table_table_.end());
}
bool Reader::CheckArrayOfTablePathPrefix() {
    auto pos = raw_path_.find_last_of('.');
    if (pos == std::string::npos) {
        return false;
    }
    // eg: [[A.B.C]]
    //  prefix = A.B
    //
    std::string prefix = raw_path_.substr(0, pos);
    return CheckArrayOfTableExists(prefix);
}

std::string Reader::ComplexPathPrefix(const std::string &path) {
    auto pos = path.find_last_of('.');
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(0, pos);
}

} // namespace internal
} // namespace TOML
