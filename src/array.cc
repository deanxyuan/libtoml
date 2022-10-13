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

#include "reader.h"
#include <math.h>
#include <string.h>
#include <vector>
#include "common.h"

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
} // namespace internal
} // namespace TOML
