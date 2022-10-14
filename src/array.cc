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
#include <vector>
#include "src/common.h"

namespace TOML {
namespace internal {

bool Reader::GetArrayImpl() {

    const int depth = StackDepth();

    uint32_t c = 0;
    Node node;

    bool find_a_separator = true;

    while (remaining_input_ > 0) {
        c = *input_;
        switch (c) {
        case '\r':
        case '\n':
            break;
        case '#':
            if (!SkipComment()) {
                goto __exit;
            }
            break;
        case ',':
            input_++;
            remaining_input_--;
            find_a_separator = true;
            break;
        case '\'':
        case '\"':
            // string
            if (!find_a_separator || !GetStringValue()) {
                goto __exit;
            }
            find_a_separator = false;
            break;
        case '+':
        case '-':
            if (!find_a_separator || !GetNumberWithPrefix()) {
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
            if (!find_a_separator || !GetNumberNoPrefix()) {
                goto __exit;
            }
            find_a_separator = false;
            break;
        case 'n': // nan
            if (find_a_separator && StartsWith("nan")) {
                node = Node::CreateFloat(NAN);
                UpdateNode(node);
                state_ = PARSE_STATUS_SUCCESS;
                input_ += 3;
                remaining_input_ -= 3;
                find_a_separator = false;
                break;
            }
            goto __exit;
        case 'i': // inf
            if (find_a_separator && StartsWith("inf")) {
                node = Node::CreateFloat(INFINITY);
                UpdateNode(node);
                state_ = PARSE_STATUS_SUCCESS;
                input_ += 3;
                remaining_input_ -= 3;
                find_a_separator = false;
                break;
            }
            goto __exit;
        case 't': // true
            if (find_a_separator && StartsWith("true")) {
                node = Node::CreateBoolean(true);
                UpdateNode(node);
                state_ = PARSE_STATUS_SUCCESS;
                input_ += 4;
                remaining_input_ -= 4;
                find_a_separator = false;
                break;
            }
            goto __exit;
        case 'f': // false
            if (find_a_separator && StartsWith("false")) {
                node = Node::CreateBoolean(false);
                UpdateNode(node);
                state_ = PARSE_STATUS_SUCCESS;
                input_ += 5;
                remaining_input_ -= 5;
                find_a_separator = false;
                break;
            }
            goto __exit;
        case '[':
            // array
            if (!find_a_separator) {
                goto __exit;
            }
            PushEmptyArray();
            input_++;
            remaining_input_--;
            break;
        case '{':
            // inlined object
            if (!find_a_separator || !GetInlineTableImpl()) {
                goto __exit;
            }
            find_a_separator = false;
            break;
        case ']':
            if (!PopStack(Types::TOML_ARRAY)) {
                goto __exit;
            }
            input_++;
            remaining_input_--;
            if (StackDepth() == depth) {
                state_ = PARSE_STATUS_SUCCESS;
                goto __exit;
            }
            break;
        default:
            goto __exit;
            break;
        }
        c = SkipFrontSpace();
        if (c == READ_CHAR_EOF) {
            break;
        }
        state_ = PARSE_STATUS_ERROR;
    } // end while
__exit:
    return state_ == PARSE_STATUS_SUCCESS;
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

} // namespace internal
} // namespace TOML
