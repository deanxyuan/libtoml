#include "src/reader.h"
#include <cmath>
#include "src/common.h"

namespace TOML {
namespace internal {
void Reader::GetNumberWithPrefix() {
    bool negative = (*input_++ == '-');
    if (--remaining_input_ == 0) return;

    uint8_t c = *input_;

    Node node;
    switch (c) {
    case 'i': //+inf -inf
        if (StartsWith("inf")) {
            node   = Node::CreateDouble(negative ? (INFINITY * (-1)) : INFINITY);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 3;
            remaining_input_ -= 3;
        }
        break;
    case 'n': //+nan -nan
        if (StartsWith("nan")) {
            node   = Node::CreateDouble(negative ? (NAN * (-1)) : NAN);
            state_ = PARSE_STATUS_SUCCESS;
            input_ += 3;
            remaining_input_ -= 3;
        }
        break;
    default:
        // 有+-号前缀，因此只能为十进制整数或浮点数
        if (TestNumberIsFloat()) {
            if (GetFloatNumber()) {
                auto value = StringToDouble();
                // 正负处理
                node   = Node::CreateDouble(negative ? (value * (-1)) : value);
                state_ = PARSE_STATUS_SUCCESS;
            }
        } else {
            if (GetDecimalNumber()) {
                node   = negative ? Node::CreateInteger(StringToInt() * (-1))
                                  : Node::CreateInteger(StringToUInt());
                state_ = PARSE_STATUS_SUCCESS;
            }
        }
        break;
    }

    if (node) UpdateNode(node);
}

void Reader::GetNumberValueImpl() {
    Node node;
    if (StartsWith("0x")) { // hex
        if (GetHexNumber()) {
            node = Node::CreateInteger(StringToUInt(16));
        }
    } else if (StartsWith("0b")) { // binary
        if (GetBinaryNumber()) {
            node = Node::CreateInteger(StringToUInt(2));
        }
    } else if (StartsWith("0o")) { // oct
        if (GetOctNumber()) {
            node = Node::CreateInteger(StringToUInt(8));
        }
    } else {
        // float, decimal, datetime
        Types type = TestPossibleType();
        switch (type) {
        case TOML::Types::TOML_INTEGER:
            if (GetDecimalNumber()) {
                node = Node::CreateInteger(StringToUInt());
            }
            break;
        case TOML::Types::TOML_DOUBLE:
            if (GetFloatNumber()) {
                node = Node::CreateDouble(StringToDouble());
            }
            break;
        default:
            if (GetDateTimeImpl()) {
                node = Node::CreateDateTime(strings_);
            }
            break;
        }
    }
    if (node) UpdateNode(node);
}
bool Reader::GetDecimalNumber() {
    bool next_must_be_num = true;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        switch (c) {
        case '_':
            if (next_must_be_num) {
                goto __exit;
            }
            next_must_be_num = true;
            break;
        case '0':
            if (strings_.empty()) {
                // 前导零是不允许的，判断是否为一个单独的 "0"
                if (remaining_input_ > 1 && !IsSpaceOrNextLine(input_[1]) && input_[1] != '#') {
                    goto __exit;
                }
            }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (next_must_be_num) {
                next_must_be_num = false;
            }
            StringAddChar(c);
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case '#': // 开始注释
            if (!next_must_be_num) {
                state_ = PARSE_STATUS_SUCCESS;
            }
            goto __exit;
        default:
            goto __exit;
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    if (state_ == PARSE_STATUS_SUCCESS) {
        // 判断数据格式是否正确
        if (strings_.size() > 1 && strings_[0] == '0') {
            state_ = PARSE_STATUS_ERROR;
            input_ -= strings_.size();
            remaining_input_ += strings_.size();
            return false;
        }
        return true;
    }
    return false;
}

bool Reader::GetFloatNumber() {
    bool next_must_be_num = true;

    bool found_dot = false; // .
    bool found_E   = false; // e E
    bool found_pm  = false; // + -

    uint32_t prev_char = 0;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        switch (c) {
        case '_':
            if (next_must_be_num) {
                goto __exit;
            }
            next_must_be_num = true;
            break;
        case '.':
            if (next_must_be_num || found_dot || found_E) {
                goto __exit;
            }
            found_dot        = true;
            next_must_be_num = true;
            StringAddChar(c);
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
            if (next_must_be_num) {
                next_must_be_num = false;
            }
            StringAddChar(c);
            break;
        case 'e':
        case 'E':
            // 读取小数精度
            if (next_must_be_num || found_E) {
                goto __exit;
            }
            // E后面不能有空格，只能为数字或+-
            if (remaining_input_ <= 1) {
                goto __exit;
            }
            if (input_[1] != '+' && input_[1] != '-' && (input_[1] < '0' || input_[1] > '9')) {
                goto __exit;
            }
            StringAddChar(c);
            found_E = true;
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case '#': // 开始注释
            if (!next_must_be_num) {
                state_ = PARSE_STATUS_SUCCESS;
            }
            goto __exit;
        case '+':
        case '-':
            if (found_pm || next_must_be_num) {
                goto __exit;
            }
            prev_char = LastInsertChar();
            if (prev_char == READ_CHAR_EOF || (prev_char != 'e' && prev_char != 'E')) {
                goto __exit;
            }
            next_must_be_num = true;
            StringAddChar(c);
            found_pm = true;
            break;
        default:
            goto __exit;
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    if (state_ == PARSE_STATUS_SUCCESS) {
        // 判断小数点之前(已添加的)数据格式是否正确
        if (strings_.size() > 1) {
            if (strings_[0] == '0' && strings_[1] != '.') {
                state_ = PARSE_STATUS_ERROR;
                input_ -= strings_.size();
                remaining_input_ += strings_.size();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool Reader::GetHexNumber() {
    // skip prefix 0x
    input_ += 2;
    remaining_input_ -= 2;

    bool next_must_be_num = true;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        switch (c) {
        case '_':
            if (next_must_be_num) {
                goto __exit;
            }
            next_must_be_num = true;
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
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            if (next_must_be_num) {
                next_must_be_num = false;
            }
            StringAddChar(c);
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case '#': // 开始注释
            if (!next_must_be_num) {
                state_ = PARSE_STATUS_SUCCESS;
            }
            goto __exit;
        default:
            goto __exit;
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    return state_ == PARSE_STATUS_SUCCESS;
}

bool Reader::GetBinaryNumber() {
    // skip prefix 0b
    input_ += 2;
    remaining_input_ -= 2;

    bool next_must_be_num = true;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        switch (c) {
        case '_':
            if (next_must_be_num) {
                goto __exit;
            }
            next_must_be_num = true;
            break;
        case '0':
        case '1':
            if (next_must_be_num) {
                next_must_be_num = false;
            }
            StringAddChar(c);
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case '#': // 开始注释
            if (!next_must_be_num) {
                state_ = PARSE_STATUS_SUCCESS;
            }
            goto __exit;
        default:
            goto __exit;
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    return state_ == PARSE_STATUS_SUCCESS;
}
bool Reader::GetOctNumber() {
    // skip prefix 0x
    input_ += 2;
    remaining_input_ -= 2;

    bool next_must_be_num = true;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        switch (c) {
        case '_':
            if (next_must_be_num) {
                goto __exit;
            }
            next_must_be_num = true;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            if (next_must_be_num) {
                next_must_be_num = false;
            }
            StringAddChar(c);
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case '#': // 开始注释
            if (!next_must_be_num) {
                state_ = PARSE_STATUS_SUCCESS;
            }
            goto __exit;
        default:
            goto __exit;
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    return state_ == PARSE_STATUS_SUCCESS;
}

bool Reader::TestNumberIsFloat() {
    for (size_t i = 0; i < remaining_input_; i++) {
        uint8_t c = input_[i];

        if (IsSpaceOrNextLine(c)) {
            break;
        }
        if (c == '#') {
            break;
        }
        if (c == '.' || c == 'e' || c == 'E') {
            return true;
        }
    }
    return false;
}

double Reader::StringToDouble() {
    char *endptr = nullptr;
    return std::strtod(strings_.data(), &endptr);
}

int64_t Reader::StringToInt(int radix) {
    char *endptr = nullptr;
    return std::strtoll(strings_.data(), &endptr, radix);
}
uint64_t Reader::StringToUInt(int radix) {
    char *endptr = nullptr;
    return std::strtoull(strings_.data(), &endptr, radix);
}
Types Reader::TestPossibleType() {

    Types type = Types::TOML_INTEGER;

    for (size_t i = 0; i < remaining_input_; i++) {
        uint8_t c = input_[i];

        switch (c) {
        case 'E':
        case 'e':
        case '.':
            type = Types::TOML_DOUBLE;
            goto __exit;
        case 'T':
        case '-':
        case ':':
            type = Types::TOML_DATETIME;
            goto __exit;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            goto __exit;
        default:
            break;
        }
    }
__exit:
    return type;
}
} // namespace internal
}; // namespace TOML
