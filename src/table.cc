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
#include <iostream>
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

} // namespace internal
} // namespace TOML
