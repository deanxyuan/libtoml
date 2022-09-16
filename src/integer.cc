#include "toml/impl/reader.h"
#include <cmath>
#include "src/common.h"

namespace TOML {
void TOMLReader::GetNumberWithPrefix() {
    bool negative = (*input_++ == '-');
    if (--remaining_input_ == 0) return;

    uint8_t c = *input_;
    int count = 0;

    switch (c) {
    case 'i': //+inf -inf
        if (!StartsWith("inf")) {
            goto __exit;
        }
        d_value_ = INFINITY;
        state_   = PARSE_STATUS_SUCCESS;
        break;
    case 'n': //+nan -nan
        if (!StartsWith("nan")) {
            goto __exit;
        }
        d_value_ = NAN;
        state_   = PARSE_STATUS_SUCCESS;
        break;
    default:
        // 因为有+-号前缀，因此只能为十进制整数或浮点数
        count = TestSameCharCountEx('0', '_');
        if (count > 1) {
            // 前导零是不允许的
            goto __exit;
        }
        if (count == 0) {
            if (TestNumberIsFloat()) {
                GetFloatNumber(negative);
            } else {
                GetDecimalNumber(negative);
            }
        } else {
            // 只能为浮点数 0.xxxx
            if (StartsWith("0.")) {
                GetFloatNumber(negative);
            }
        }
        break;
    }
__exit:
    if (state_ == PARSE_STATUS_SUCCESS) {
    }
}

void TOMLReader::GetDecimalNumber(bool negative) {
    bool next_must_be_num = true;

    char *endptr = nullptr;

    uint8_t c = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        if (next_must_be_num) {
            if (c < '0' || c > '9') {
                goto __exit;
            }

            next_must_be_num = false;
            StringAddChar(c);
            input_++;
            remaining_input_--;
            continue;
        }
        switch (c) {
        case '_':
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
            StringAddChar(c);
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            state_ = PARSE_STATUS_SUCCESS;
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
        if (negative) {
            i_value_ = std::strtoll(strings_.c_str(), &endptr, 10) * (-1);
        } else {
            u_value_ = std::strtoll(strings_.c_str(), &endptr, 10);
        }
    }
}

void TOMLReader::GetFloatNumber(bool negative) {
    bool next_must_be_num = true;

    bool found_dot = false;
    bool found_E   = false;

    char *endptr = nullptr;
    uint8_t c    = 0;

    while (remaining_input_ > 0) {
        c = *input_;

        if (next_must_be_num) {
            if (c < '0' || c > '9') {
                goto __exit;
            }
            next_must_be_num = false;
            StringAddChar(c);
            input_++;
            remaining_input_--;
            continue;
        }
        switch (c) {
        case '_':
            next_must_be_num = true;
            break;
        case '.':
            if (found_dot) {
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
            StringAddChar(c);
            break;
        case 'e':
        case 'E':
            // 读取小数精度
            if (found_E) {
                goto __exit;
            }
            found_E = true;
            break;
        case '\t':
        case '\r':
        case '\n':
        case ' ':
            state_ = PARSE_STATUS_SUCCESS;
            break;
        case '+':
        case '-':
            break;
        default:
            break;
        }
        input_++;
        remaining_input_--;
    }
__exit:
    if (state_ == PARSE_STATUS_SUCCESS) {
        if (negative) {
            i_value_ = std::strtoll(strings_.c_str(), &endptr, 10);
        } else {
            u_value_ = std::strtoll(strings_.c_str(), &endptr, 10);
        }
    }
}

bool TOMLReader::TestNumberIsFloat() {
    const uint8_t *backup = input_;
    size_t backup_size    = remaining_input_;

    while (backup_size > 0) {
        uint8_t c = *backup++;
        backup_size--;

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            break;
        }
        if (c == '.') {
            return true;
        }
    }
    return false;
}
int TOMLReader::TestSameCharCountEx(uint8_t ch, uint8_t skip) {
    int count = 0;
    for (size_t i = 0; i < remaining_input_; i++) {
        if (input_[i] == ch) {
            count++;
            continue;
        }
        if (input_[i] == skip) {
            continue;
        }
        break;
    }
    return count;
}
}; // namespace TOML
