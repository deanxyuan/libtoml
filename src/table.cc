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
#include <math.h>
#include <string.h>
#include "src/common.h"

namespace TOML {
namespace internal {

bool Reader::GetInlineTableImpl() {
    int depth = StackDepth();

    state_ = PARSE_STATUS_ERROR;

    bool find_a_separator  = false;
    bool need_value_or_key = true; // true:value, false:key

    uint32_t c = 0;
    Node node;

    while (remaining_input_ > 0) {
        c = *input_;
        if (need_value_or_key) {
            switch (c) {
            case ' ':
            case '\t':
                input_++;
                remaining_input_--;
                break;
            case '\r':
            case '\n':
            case '#':
                goto __exit;
            case ',':
                input_++;
                remaining_input_--;
                find_a_separator  = true;
                need_value_or_key = false;
                break;
            case '\'':
            case '\"':
                // string
                if (!GetStringValue()) {
                    goto __exit;
                }
                find_a_separator = false;
                break;
            case '+':
            case '-':
                if (!GetNumberWithPrefix()) {
                    goto __exit;
                }
                find_a_separator = false;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                // integer float datetime
                if (!GetNumberNoPrefix()) {
                    goto __exit;
                }
                find_a_separator = false;
                break;
            case 'n': // nan
                if (StartsWith("nan")) {
                    node   = Node::CreateFloat(NAN);
                    state_ = PARSE_STATUS_SUCCESS;
                    input_ += 3;
                    remaining_input_ -= 3;
                    find_a_separator = false;
                    break;
                }
                goto __exit;
            case 'i': // inf
                if (StartsWith("inf")) {
                    node   = Node::CreateFloat(INFINITY);
                    state_ = PARSE_STATUS_SUCCESS;
                    input_ += 3;
                    remaining_input_ -= 3;
                    find_a_separator = false;
                    break;
                }
                goto __exit;
            case 't': // true
                if (StartsWith("true")) {
                    node   = Node::CreateBoolean(true);
                    state_ = PARSE_STATUS_SUCCESS;
                    input_ += 4;
                    remaining_input_ -= 4;
                    find_a_separator = false;
                    break;
                }
                goto __exit;
            case 'f': // false
                if (StartsWith("false")) {
                    node   = Node::CreateBoolean(false);
                    state_ = PARSE_STATUS_SUCCESS;
                    input_ += 5;
                    remaining_input_ -= 5;
                    find_a_separator = false;
                    break;
                }
                goto __exit;
            case '{':
                PushEmptyObject();
                input_++;
                remaining_input_--;
                find_a_separator  = false;
                need_value_or_key = false;
                break;
            case '[':
                if (!GetArrayImpl()) {
                    goto __exit;
                }
                find_a_separator = false;
                break;
            case '}':
                if (find_a_separator || !InlinedTablePop()) {
                    goto __exit;
                }
                input_++;
                remaining_input_--;
                if (StackDepth() == depth) {
                    state_ = PARSE_STATUS_SUCCESS;
                    goto __exit;
                }
                find_a_separator = false;
                break;
            default:
                goto __exit;
                break;
            }
        } else {
            // find key and '='
            if (c == ' ' || c == '\t') {
                input_++;
                remaining_input_--;
            } else if (c == '\'' || c == '\"' || IsValidCharForRawKey(c)) {
                if (!ParseComplexKey()) {
                    goto __exit;
                }
                depth += static_cast<int>(current_.key_path.size() - 1);
                if (!UsingComplexKey() || !CheckSeparator()) {
                    goto __exit;
                }
                need_value_or_key = true;
            } else {
                goto __exit;
            }
        }
        state_ = PARSE_STATUS_ERROR;
    } // end while
__exit:
    return state_ == PARSE_STATUS_SUCCESS;
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
            assert(node.Type() == Types::TOML_OBJECT);
            int count = static_cast<int>(current_.title_path.size());
            for (int i = 1; i < count; i++) {
                Node obj = node.As<kObject>()->Get(current_.title_path[i]);
                if (obj) {
                    if (obj.Type() != Types::TOML_OBJECT) {
                        desc_ = "Node \"" + current_.title_path[i] + "\" definition conflict";
                        return false;
                    }
                    node = obj;
                } else {
                    obj = Node::CreateObject();
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
bool Reader::CheckTableTileRedefine() {
    auto it = table_map_.find(current_.title);
    return (it != table_map_.end());
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
