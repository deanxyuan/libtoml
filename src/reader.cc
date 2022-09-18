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
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

#include "src/common.h"
#include "toml/impl/reader.h"

namespace TOML {
namespace internal {

Reader::Reader(const char *data, size_t len)
    : original_input_(reinterpret_cast<const uint8_t *>(data))
    , input_(original_input_)
    , remaining_input_(len)
    , state_(TOML::PARSE_STATUS_NULL) {}

Reader::~Reader() {}

void Reader::SetKey() {
    key_ = std::move(strings_);
    strings_.clear();
}

void Reader::StringAddChar(uint32_t c) { strings_.push_back(static_cast<uint8_t>(c)); }
void Reader::StringAddPtr(const char *ptr) { strings_.append(ptr); }
void Reader::StringAddPtr(const char *ptr, size_t count) { strings_.append(ptr, count); }
void Reader::StringAddCharEx(int count, char ch) { strings_.append(count, ch); }
void Reader::StringAddString(const std::string &s) { strings_.append(s); }
uint32_t Reader::LastInsertChar() {
    size_t count = strings_.size();
    return count == 0 ? READ_CHAR_EOF : strings_[count - 1];
}
bool Reader::IsValidCharForRawKey(uint32_t c) {
    if (c == '-' || c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z')) {
        return true;
    }
    return false;
}
bool Reader::IsValidCharForHex(uint32_t c) {
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
        return true;
    }
    return false;
}

bool Reader::IsSpaceOrNextLine(uint32_t c) {
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        return true;
    }
    return false;
}

bool Reader::StartsWith(const char *prefix) {
    size_t prefix_len = strlen(prefix);
    return remaining_input_ >= prefix_len && (memcmp(input_, prefix, prefix_len) == 0);
}

void Reader::Run() {

    root_ = Node::CreateObject();
    stack_.push(root_);

    uint8_t c = 0;

    while (remaining_input_ > 0) {

        // 应当认为，每次循环都要获取一条有效数据,
        // 有效数据有三种情况：
        // 1. key=value
        // 2. [table]
        // 3. array = []
        // 4. object = {} 内联对象
    __re_search:
        // step 1 : 跳过前缀空格
        while (remaining_input_ > 0) {
            c = *input_;

            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                input_++;
                remaining_input_--;
                continue;
            }
            break;
        }

        if (remaining_input_ == 0) {
            state_ = PARSE_STATUS_SUCCESS;
            goto __exit;
        }

        // step 2 : 解析一组有效数据 (一行或多行)
        switch (c) {
        case '#':
            // 当前注释，找下一个换行
            do {
                c = *++input_;
                remaining_input_--;
                if (c == '\r' || c == '\n') {
                    break;
                }

                // 除制表符以外的控制字符（U+0000 至 U+0008，U+000A 至
                // U+001F，U+007F）不允许出现在注释中
                if ((0 <= c && c <= 0x08) || (0x0a <= c && c <= 0x1f) || (c == 0x7f)) {
                    goto __exit;
                }
            } while (remaining_input_ > 0);
            goto __re_search;
        case '\'':
        case '\"':
            // 找 key 的结束符 key=value
            if (!GetKeyImpl(c) || !CheckSeparator() || !GetValueImpl()) {
                goto __exit;
            }
            break;
        case '[':
            // 找 table 的结束符 [table]
            break;
        default:
            // 三种情况：
            // 1.raw-key = value
            // 2.array = []
            // 3.object = {}
            if (!GetRawKeyImpl() || !CheckSeparator() || !GetValueImpl()) {
                goto __exit;
            }
            break;
        }

        // step 3 : 验证该组有效数据后缀
        while (remaining_input_ > 0) {
            c = *input_;
            if (c == ' ' || c == '\t' || c == '\r') {
                input_++;
                remaining_input_--;
                continue;
            }
            if (c == '\n' || c == '#') {
                break;
            }
            state_ = PARSE_STATUS_ERROR;
            goto __exit;
        }
    }
__exit:
    if (state_ != PARSE_STATUS_SUCCESS) {
        error_ = std::string("TOML parse error at index ") + std::to_string(CurrentIndex());
    }
}

bool Reader::GetKeyImpl(uint8_t flag) {
    if (remaining_input_ == 1) {
        state_ = PARSE_STATUS_ERROR;
        return false;
    }

    input_++;
    remaining_input_--;

    state_ = PARSE_STATUS_ERROR;

    if (flag == '\'') {
        GetLiteralString();
    } else {
        GetBasicString();
    }

    if (state_ == PARSE_STATUS_SUCCESS) {
        SetKey();
        return true;
    }
    return false;
}

bool Reader::GetRawKeyImpl() {

    state_ = PARSE_STATUS_ERROR;

    uint8_t c;
    const char target[] = " =\t";

    while (remaining_input_ > 0) {
        c = *input_;

        if (c == '\r' || c == '\n') {
            break;
        }
        if (IsByteExistsInTarget(target, c)) {
            state_ = PARSE_STATUS_SUCCESS;
            break;
        }

        if (!IsValidCharForRawKey(c)) {
            break;
        }
        StringAddChar(c);
        input_++;
        remaining_input_--;
    }

    if (state_ == PARSE_STATUS_SUCCESS) {
        SetKey();
        return true;
    }
    return false;
}

bool Reader::CheckSeparator() {

    state_ = PARSE_STATUS_ERROR;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        // 键值对要求等号和键必须在同一行
        if (c == '\r' || c == '\n') {
            break;
        }
        if (c == ' ' || c == '\t') {
            input_++;
            remaining_input_--;
            continue;
        }

        if (c == '=') {
            input_++;
            remaining_input_--;
            state_ = PARSE_STATUS_SUCCESS;
            break;
        }
        break;
    }

    return (state_ == PARSE_STATUS_SUCCESS);
}

bool Reader::GetValueImpl() {

    state_ = PARSE_STATUS_ERROR;

    uint8_t c;
    Node node;

    while (remaining_input_ > 0) {
        c = *input_;

        // 键值对要求键、等号和值必须在同一行，
        // 部分值可以跨多行
        if (c == '\r' || c == '\n') {
            break;
        }
        if (c == ' ' || c == '\t') {
            input_++;
            remaining_input_--;
            continue;
        }

        // 找到不为空的字符
        state_ = PARSE_STATUS_SUCCESS;
        break;
    }

    if (state_ == PARSE_STATUS_ERROR) {
        return false;
    }
    state_ = PARSE_STATUS_ERROR;

    switch (c) {
    case '\'':
    case '\"':
        // 字符串或多行字符串
        GetStringValueImpl();
        break;
    case '+':
    case '-':
        GetNumberWithPrefix();
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
        // 整数、浮点、或时间
        GetNumberValueImpl();
        break;
    case 'n': // nan
        if (StartsWith("nan")) {
            node   = Node::CreateFloat(NAN);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 3;
            remaining_input_ -= 3;
        }
    case 'i': // inf
        if (StartsWith("inf")) {
            node   = Node::CreateFloat(INFINITY);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 3;
            remaining_input_ -= 3;
        }
        break;
    case 't': // true
        if (StartsWith("true")) {
            node   = Node::CreateBoolean(true);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 4;
            remaining_input_ -= 4;
        }
        break;
    case 'f': // false
        if (StartsWith("false")) {
            node   = Node::CreateBoolean(false);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 5;
            remaining_input_ -= 5;
        }
        break;
    case '[':
        // 数组
        break;
    case '{':
        // 对象
        break;
    default:
        break;
    }

    if (node) UpdateNode(node);
    return state_ == PARSE_STATUS_SUCCESS;
}

void Reader::UpdateKeyValue() { UpdateNode(Node::CreateString(strings_)); }

void Reader::UpdateNode(Node node) {
    Node &parent = stack_.top();
    if (parent.Type() == Types::TOML_ARRAY) {
        parent.As<kArray>()->PushBack(node);
    } else {
        parent.As<kObject>()->Insert(key_, node);
    }
    key_.clear();
    strings_.clear();
}

std::string Reader::Parse(const char *data, size_t len, Node *node) {
    Reader reader(data, len);
    reader.Run();
    if (reader.Result() != PARSE_STATUS_SUCCESS) {
        return reader.error_;
    }

    std::swap(*node, reader.root_);
    return std::string();
}
} // namespace internal

Node LoadFromFile(const std::string &path, std::string *error) {
    std::ifstream file(path, std::ios_base::in);
    if (!file.is_open()) {
        if (error) *error = "Failed to open " + path;
        return Node();
    }

    file.seekg(0, std::ios_base::end);
    std::streamoff file_size = file.tellg();
    file.seekg(0, std::ios_base::beg);
    if (file_size < 0) {
        if (error) *error = std::string("Failed to get file size");
        return Node();
    } else if (file_size == 0) {
        if (error) *error = std::string("It's a empty file");
        return Node();
    }

    std::unique_ptr<char[]> buff(new char[file_size]);
    file.read(buff.get(), file_size);
    if (!file.good()) {
        if (error) *error = std::string("Failed to read file data");
        file.close();
        return Node();
    }
    file.close();
    return LoadFromData(buff.get(), file_size, error);
}

Node LoadFromData(const char *data, size_t len, std::string *error) {
    Node node;
    std::string desc = internal::Reader::Parse(data, len, &node);
    if (error) *error = desc;
    return node;
}
} // namespace TOML
