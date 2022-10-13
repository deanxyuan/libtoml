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
            desc_ = "object \"" + prefix + "\" redefine";
            return false;
        }
    }

    // 点分隔键为最后一个键名前的每个键名创建并定义一个表，
    // 倘若这些表尚未被创建的话
    complex_key_depth_ = 0;

    Node parent = stack_.top();
    int count   = static_cast<int>(current_.key_path.size());
    for (int i = 0; i < count - 1; i++) {
        assert(parent.Type() == Types::TOML_OBJECT);
        auto key = current_.key_path[i];
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

    assert(parent.Type() == Types::TOML_OBJECT);
    auto key = GetVectorLastElement(current_.key_path);
    if (parent.As<kObject>()->Exists(key)) {
        desc_ = "\"" + key + "\" already exists";
        return false;
    }
    std::swap(key_, key);
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
    std::string title;

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
            title.append(strings_);
            title.push_back('.');
            path.push_back(strings_);
            strings_.clear();
            find_a_separator = true;
            break;
        case ']':
            if (find_a_separator) {
                goto __exit;
            }
            title.append(strings_);
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
    if (!result) path.clear();
    std::swap(current_.title_path, path);
    std::swap(current_.title, title);
    return result;
}

bool Reader::UsingTableTitle() {
    return is_table_title_ ? UsingTableTitleImpl() : UsingArrayOfTableTitleImpl();
}

bool Reader::UsingTableTitleImpl() {

    table_depth_ = 0;

    if (!array_of_table_map_.empty() && current_.title_path.size() > 1) {
        auto it = array_of_table_map_.find(current_.title_path[0]);
        if (it != array_of_table_map_.end()) {
            // 说明是对表数组的引用
            Node node = it->second;
            int count = static_cast<int>(current_.title_path.size());
            for (int i = 1; i < count; i++) {
                assert(node.Type() == Types::TOML_OBJECT);
                if (node.As<kObject>()->Exists(current_.title_path[i])) {
                    node = node.As<kObject>()->Get(current_.title_path[i]);
                } else {
                    Node obj = Node::CreateObject();
                    node.As<kObject>()->Insert(current_.title_path[i], obj);
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
    auto iter = table_map_.find(current_.title);
    if (iter != table_map_.end()) {
        desc_ = "object [" + current_.title + "] redefine";
        return false;
    }

    // 路径前缀处理，每个前缀节点都是一个OBJECT
    std::string prefix;
    Node parent = stack_.top();
    int count   = static_cast<int>(current_.title_path.size());
    for (int i = 0; i < count - 1; i++) {
        auto key = current_.title_path[i];
        Node obj = parent.As<kObject>()->Get(key);
        if (!obj) {
            obj = Node::CreateObject();
            parent.As<kObject>()->Insert(key, obj);
        } else if (obj.Type() != Types::TOML_OBJECT) {
            desc_ = "object " + prefix + key + " redefine";
            return false;
        } else if (obj.As<kObject>()->Inlined()) {
            desc_ = "\"" + key + "\" is an inline table and cannot be modified";
            return false;
        }
        parent = obj;
        PushStack(obj);
        prefix.append(key);
        prefix.push_back('.');
    }

    auto key = GetVectorLastElement(current_.title_path);
    Node obj = parent.As<kObject>()->Get(key);
    if (!obj) {
        obj = Node::CreateObject();
        parent.As<kObject>()->Insert(key, obj);
    } else if (obj.Type() != Types::TOML_OBJECT) {
        desc_ = "node \"" + current_.title + "\" redefine";
        return false;
    } else if (obj.As<kObject>()->Inlined()) {
        desc_ = "object [" + current_.title + "] is an inline table and cannot be modified";
        return false;
    }
    table_map_[current_.title] = DISABLE_REPEATED; // 记录该对象的路径，用于检测重定义
    PushStack(obj);
    table_depth_ = count;
    return true;
}
bool Reader::UsingArrayOfTableTitleImpl() {

    if (current_.title_path.size() == 1) {
        // 表数组元素定义
        std::string key = current_.title_path[0];
        Node sub_array, parent = stack_.top();
        assert(parent.Type() == Types::TOML_OBJECT);

        if (!parent.As<kObject>()->Exists(key)) {
            // 首次出现，即第一个元素
            sub_array = Node::CreateArray();
            parent.As<kObject>()->Insert(key, sub_array);
        } else {
            // 非首次出现，表示插入新的元素
            sub_array = parent.As<kObject>()->Get(key);
            if (sub_array.Type() != Types::TOML_ARRAY) {
                desc_ = "\"" + key + "\" already exists and it is not an array";
                return false;
            }
        }
        // 创建一个空对象，入栈
        Node obj = Node::CreateObject();
        PushStack(obj);
        sub_array.As<kArray>()->PushBack(obj);
        // 保存父节点(ARRAY)的最新元素
        array_of_table_map_[key] = obj;
        // 更新栈高度
        table_depth_ = 1;
        return true;
    }
    // 嵌套表数组，判断路径是否有效
    // [[A.B.C]] 判断前缀 A.B 是否存在
    auto prefix = ComplexPathPrefix(current_.title_path);
    auto iter   = array_of_table_map_.find(prefix);
    if (iter == array_of_table_map_.end()) {
        desc_ = "Missing definition of parent node \"" + prefix + "\"";
        return false;
    }

    // 获取最新的节点名, 如 [[A.B.C]] 中的 C
    auto key = GetVectorLastElement(current_.title_path);

    // 获取路径前缀的最后一次更新的 OBJECT
    Node sub_array, last_node = iter->second;

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
    PushStack(obj);
    sub_array.As<kArray>()->PushBack(obj);
    array_of_table_map_[current_.title] = obj;

    // 更新栈高度
    table_depth_ = 1;
    return true;
}
bool Reader::CheckTableTileRedefine() {
    auto it = table_map_.find(current_.title);
    return (it != table_map_.end());
}
bool Reader::CheckArrayOfTableExists(const std::string &name) {
    auto it = array_of_table_map_.find(name);
    return (it != array_of_table_map_.end());
}

std::string Reader::ComplexPathPrefix(const std::vector<std::string> &vec) {
    std::string res;
    int count = static_cast<int>(vec.size());
    for (int i = 0; i < count - 1; i++) {
        res.append(vec[i]);
        res.push_back('.');
    }
    if (!res.empty()) {
        res.resize(res.size() - 1);
    }
    return res;
}
std::string Reader::GetVectorLastElement(const std::vector<std::string> &vec) {
    if (vec.empty()) return std::string();
    return vec[vec.size() - 1];
}
void Reader::DisablePrevTable() {
    for (auto it = table_map_.begin(); it != table_map_.end(); ++it) {
        if (it->second == ALLOWED_REPEATED) {
            it->second = DISABLE_REPEATED;
        }
    }
}
} // namespace internal
} // namespace TOML
