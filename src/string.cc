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
#include "src/common.h"

namespace TOML {
namespace internal {

bool Reader::GetStringValue() {
    if (GetStringValueImpl()) {
        Node node = Node::CreateString(strings_);
        UpdateNode(node);
        return true;
    }
    return false;
}

bool Reader::GetStringValueImpl() {
    uint8_t c = *input_;
    if (c == '\'') {
        if (StartsWith("\'\'\'")) {
            input_ += 3;
            remaining_input_ -= 3;
            return GetMultiLineLiteralString();
        }
        input_ += 1;
        remaining_input_ -= 1;
        return GetLiteralString();
    }
    if (StartsWith("\"\"\"")) {
        input_ += 3;
        remaining_input_ -= 3;
        return GetMultiLineBasicString();
    }
    input_ += 1;
    remaining_input_ -= 1;
    return GetBasicString();
}

// 单行字面量字符串解析
bool Reader::GetLiteralString() {
    bool result = false;
    uint8_t c   = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        if (!IsValidCharForString(false, c)) {
            break;
        }
        // 单行字面量不允许换行
        if (c == '\r' || c == '\n') {
            break;
        }
        if (c == '\'') {
            input_++;
            remaining_input_--;
            result = true;
            break;
        }
        input_++;
        remaining_input_--;
        StringAddChar(c);
    }

    return result;
}

// 多行字面量字符串两侧各有三个单引号来包裹，允许换行。
// 类似于字面量字符串，无论任何转义都不存在。
bool Reader::GetMultiLineLiteralString() {
    // 至少包含 '''
    if (remaining_input_ < 3) {
        return false;
    }

    // 紧随开始标记的那个换行会被剔除。 开始结束标记之间的所有其它内容会原样对待。
    if (*input_ == '\n') {
        input_++;
        remaining_input_--;
    } else if (input_[0] == '\r' && input_[1] == '\n') {
        input_ += 2;
        remaining_input_ -= 2;
    }
    bool result = false;
    uint8_t c   = 0;
    int count   = 0;
    while (remaining_input_ > 0) {
        c = *input_;

        if (!IsValidCharForString(true, c)) {
            break;
        }

        if (c == '\'') {
            // 3-5个' 表示可以结束
            count = TestSameCharCount('\'');
            if (count < 3) {
                StringAddChar(c);
            } else if (count == 3) {
                result = true;
                break;
            } else {
                StringAddCharEx(count - 3, '\'');
                result = true;
                break;
            }
        } else {
            StringAddChar(c);
        }
        input_++;
        remaining_input_--;
    }

    // 只允许单行，必须出现结束符
    if (result) {
        input_ += count;
        remaining_input_ -= count;
    }
    return result;
}

bool Reader::GetMultiLineBasicString() {
    bool transferred = false, foldup = false, result = false;
    int nhex = 0, i = 0, value = 0;
    uint8_t c = 0, ch = 0;
    int64_t ucs = 0;
    int count   = 0;
    std::string utf8;

    // 至少应该包含 """
    if (remaining_input_ < 3) {
        return false;
    }

    // 多行基本字符串由三个引号包裹，允许折行。
    // 紧随开头引号的那个换行会被去除。
    if (input_[0] == '\n') {
        input_++;
        remaining_input_--;
    } else if (input_[0] == '\r' && input_[1] == '\n') {
        input_ += 2;
        remaining_input_ -= 2;
    }

    while (remaining_input_ > 0) {
        c = *input_;

        if (!IsValidCharForString(true, c)) {
            break;
        }

        switch (c) {
        case 'b':
            if (transferred) {
                StringAddChar('\b');
                transferred = false;
            } else {
                StringAddChar('b');
            }
            break;
        case 't':
            if (transferred) {
                StringAddChar('\t');
                transferred = false;
            } else {
                StringAddChar('t');
            }
            break;
        case 'n':
            if (transferred) {
                StringAddChar('\n');
                transferred = false;
            } else {
                StringAddChar('n');
            }
            break;
        case 'f':
            if (transferred) {
                StringAddChar('\f');
                transferred = false;
            } else {
                StringAddChar('f');
            }
            break;
        case 'r':
            if (transferred) {
                StringAddChar('\r');
                transferred = false;
            } else {
                StringAddChar('r');
            }
            break;
        case '\"':
            if (transferred) {
                StringAddChar('\"');
                transferred = false;
            } else {
                // 3-5个" 表示可以结束
                count = TestSameCharCount('\"');
                if (count < 3) {
                    StringAddChar(c);
                } else if (count == 3) {
                    result = true;
                    goto __exit;
                } else {
                    StringAddCharEx(count - 3, '\"');
                    result = true;
                    goto __exit;
                }
            }
            break;
        case '\\':
            if (transferred) {
                StringAddChar('\\');
                transferred = false;
            } else {
                // 当一行的最后一个非空白字符是未被转义的 \ 时，它会连同它后面的所
                // 有空白（包括换行）一起被去除，直到下一个非空白字符或结束引号为止

                for (i = 0; i < static_cast<int>(remaining_input_) - 1; i++) {
                    ch = input_[i + 1];

                    if (!IsValidCharForString(true, ch)) {
                        goto __exit;
                    }
                    if (ch == '\t' || ch == ' ') {
                        continue;
                    }
                    if (ch == '\r' || ch == '\n') {
                        foldup = true;
                        continue;
                    }

                    // 出现明文字符
                    break;
                }
                if (foldup) {
                    input_ += i;
                    remaining_input_ -= i;
                } else {
                    transferred = true;
                }
            }
            break;
        case 'u': // 接 4 UNICODE 字符
        case 'U': // 接 8 UNICODE 字符
        {
            if (!transferred) {
                StringAddChar(c);
                break;
            }

            nhex = (c == 'u') ? 4 : 8;
            if (static_cast<int>(remaining_input_) - 1 < nhex) {
                // 数据不够
                goto __exit;
            }

            ucs = 0;
            for (i = 0; i < nhex; i++) {
                ch = *(input_ + 1);

                value = ('0' <= ch && ch <= '9')
                            ? (ch - '0')
                            : (('A' <= ch && ch <= 'F') ? (ch - 'A' + 10) : -1);
                if (-1 == value) {
                    goto __exit;
                }
                ucs = ucs * 16 + value;

                input_++;
                remaining_input_--;
            }

            if (!UCSToUTF8(ucs, &utf8)) {
                // 无效 UNICODE
                goto __exit;
            }
            StringAddString(utf8);
            transferred = false;
            break;
        }
        default:
            if (transferred) {
                // 不支持其他转义字符
                goto __exit;
            }
            StringAddChar(c);
            break;
        }
        input_++;
        --remaining_input_;
    }

__exit:

    if (result) {
        input_ += count;
        remaining_input_ -= count;
    }
    return result;
}

bool Reader::GetBasicString() {
    bool transferred = false, result = false;
    int nhex = 0, i = 0, value = 0;
    uint8_t c = 0, ch = 0;
    int64_t ucs = 0;
    std::string utf8;

    while (remaining_input_ > 0) {
        c = *input_;

        if (!IsValidCharForString(false, c)) {
            break;
        }

        switch (c) {
        case 'b':
            if (transferred) {
                StringAddChar('\b');
                transferred = false;
            } else {
                StringAddChar('b');
            }
            break;
        case 't':
            if (transferred) {
                StringAddChar('\t');
                transferred = false;
            } else {
                StringAddChar('t');
            }
            break;
        case 'n':
            if (transferred) {
                StringAddChar('\n');
                transferred = false;
            } else {
                StringAddChar('n');
            }
            break;
        case 'f':
            if (transferred) {
                StringAddChar('\f');
                transferred = false;
            } else {
                StringAddChar('f');
            }
            break;
        case 'r':
            if (transferred) {
                StringAddChar('\r');
                transferred = false;
            } else {
                StringAddChar('r');
            }
            break;
        case '\"':
            if (transferred) {
                StringAddChar('\"');
                transferred = false;
            } else {
                input_++;
                remaining_input_--;
                result = true;
                goto __exit;
            }
            break;
        case '\\':
            if (transferred) {
                StringAddChar('\\');
                transferred = false;
            } else {
                transferred = true;
            }
            break;
        case 'u': // 接 4 UNICODE 字符
        case 'U': // 接 8 UNICODE 字符
        {
            if (!transferred) {
                StringAddChar(c);
                break;
            }

            nhex = (c == 'u') ? 4 : 8;
            if (static_cast<int>(remaining_input_) - 1 < nhex) {
                // 数据不够
                goto __exit;
            }

            ucs = 0;
            for (i = 0; i < nhex; i++) {
                ch = *(input_ + 1);

                value = ('0' <= ch && ch <= '9')
                            ? (ch - '0')
                            : (('A' <= ch && ch <= 'F') ? (ch - 'A' + 10) : -1);
                if (-1 == value) {
                    goto __exit;
                }
                ucs = ucs * 16 + value;

                input_++;
                remaining_input_--;
            }

            if (!UCSToUTF8(ucs, &utf8)) {
                // 无效 UNICODE
                goto __exit;
            }
            StringAddString(utf8);
            transferred = false;
            break;
        }
        default:
            if (transferred) {
                // 不支持其他转义字符
                goto __exit;
            }
            StringAddChar(c);
            break;
        }
        input_++;
        --remaining_input_;
    }

__exit:

    return result;
}

// 多行字面量字符串中的任何位置写一个或两个单引号
// 因此字面量终止符范围3-5个'
// 多行基本字符串内的任何地方写一个引号或两个毗连的引号
// 基本字符串终止符范围3-5个"
int Reader::TestSameCharCount(uint8_t ch) {
    int count = 0;
    for (size_t i = 0; i < remaining_input_; i++) {
        if (input_[i] == ch) {
            count++;
            continue;
        }
        break;
    }

    return count;
}
} // namespace internal
} // namespace TOML
