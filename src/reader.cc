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
void Reader::StringAddChar(const char *ptr) { strings_.append(ptr); }
void Reader::StringAddChar(const char *ptr, int count) { strings_.append(ptr, count); }
void Reader::StringAddChar(int count, char ch) { strings_.append(count, ch); }
void Reader::StringAddChar(const std::string &s) { strings_.append(s); }

bool Reader::IsValidCharForRawKey(uint32_t c) {
    if (c == '-' || c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z')) {
        return true;
    }
    return false;
}

bool Reader::StartsWith(const char *prefix) {
    size_t prefix_len = strlen(prefix);
    return remaining_input_ >= prefix_len && (memcmp(input_, prefix, prefix_len) == 0);
}

void Reader::MoveForward(int offset) {
    input_ += offset;
    remaining_input_ -= offset;
}

void Reader::Run() {

    root_ = Node::CreateObject();
    stack_.push(root_);

    int offset = 0;
    uint8_t c  = 0;

    while (remaining_input_ > 0) {

        // 应当认为，每次循环都要获取一条有效数据,
        // 有效数据有三种情况：
        // 1. key=value
        // 2. [table]
        // 3. array = []
        // 4. object = {} 内联对象
    __re_search:
        // step 1 : skip space chars
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

        switch (c) {
        case '#':
            // 当前注释，找下一个换行
            offset = FindNextChar(input_, '\n', remaining_input_);
            if (offset == -1) {
                // 找不到新行，表示到结尾，解析结束
                state_ = PARSE_STATUS_SUCCESS;
                goto __exit;
            }
            MoveForward(offset + 1);
            goto __re_search;
        case '\'':
        case '\"':
            // 找 key 的结束符 key=value
            if (!GetKeyImpl(c) || !CheckSeparator() || !GetValueImpl()) {
                goto __exit;
            }
            ShowKey();
            ShowValue();
            UpdateKeyValue();
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
            ShowKey();
            ShowValue();
            UpdateKeyValue();
            break;
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
        break;
    case 'a': // ana
    case 'i': // inf
        break;
    case 't':
    case 'f':
        // 布尔值
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

    return state_ == PARSE_STATUS_SUCCESS;
}

void Reader::ShowKey() {
    std::stringstream ss;
    ss << "key:[" << key_ << "]";
    std::cout << ss.str() << std::endl;
}

void Reader::ShowValue() {
    std::stringstream ss;
    ss << "value:[" << strings_ << "]";
    std::cout << ss.str() << std::endl;
}

void Reader::UpdateKeyValue() {
    Node value = Node::CreateString(strings_);

    Node &node = stack_.top();
    node.As<kTomlObject>()->Insert(key_, value);

    key_.clear();
    strings_.clear();
}

std::string Reader::Parse(const char *data, size_t len, Node *node) {
    Reader reader(data, len);
    reader.Run();
    if (reader.Result() == PARSE_STATUS_SUCCESS) {
        return std::string("No Error");
    }

    std::swap(*node, reader.root_);
    return reader.error_;
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
