#include "src/reader.h"
#include <time.h>
#include "src/common.h"

namespace TOML {
namespace internal {
bool Reader::GetDateTimeImpl() {
    // YYYY-MM-DD or HH:MM:SS
    if (remaining_input_ < 8) return false;
    DateTime::InitDetail(&dt_);
    if (input_[2] == ':') {
        if (GetTimeImpl()) {
            state_ = PARSE_STATUS_SUCCESS;
        }
    } else if (input_[4] == '-') {
        if (GetDateImpl()) {
            state_ = PARSE_STATUS_SUCCESS;
        }
    }
    return state_ == PARSE_STATUS_SUCCESS;
}

bool Reader::GetTimeImpl() {
    // HH:MM:SS
    // HH:MM:SS[Z]
    // HH:MM:SS[+/-HH:MM]
    // HH:MM:SS[.microsecs]
    // HH:MM:SS[.microsecs][+/-HH:MM]

    if (!ReadTimeString()) {
        return false;
    }
    if (IsReachTheEnd() || IsSpaceOrNextLine(*input_) || *input_ == '#') {
        return true;
    }

    uint8_t c = *input_++;
    remaining_input_--;

    if (c == 'Z') {
        StringAddChar(c);
        if (IsReachTheEnd() || IsSpaceOrNextLine(*input_) || *input_ == '#') {
            return true;
        }
        return true;
    }

    if (c == '.') {
        // read microseconds
        StringAddChar(c);

        char buff[10] = {0};
        size_t i      = 0;

        for (; i < remaining_input_ && i < 9; i++) {
            uint8_t ch = input_[i];
            if (isdigit(ch)) {
                buff[i] = ch;
                continue;
            }
            if (IsSpaceOrNextLine(ch) || ch == '#' || ch == '+' || ch == '-') {
                break;
            }
            return false;
        }
        if (i <= 3) {
            // milliseconds
            dt_.SetMicroSecond(StringToInt(buff) * 1000);
        } else if (i <= 6) {
            // microseconds
            dt_.SetMicroSecond(StringToInt(buff));
        } else {
            // nanoseconds
            dt_.SetMicroSecond(StringToInt(buff) / 1000);
        }

        StringAddChar(buff, i);
        input_ += i;
        remaining_input_ -= i;

        if (IsReachTheEnd() || IsSpaceOrNextLine(*input_) || *input_ == '#') {
            return true;
        }
        c = *input_++;
        remaining_input_--;
    }

    if (c == '+' || c == '-') {
        StringAddChar(c);
        return ReadUTCOffsetString();
    }
    return false;
}
bool Reader::GetDateImpl() {
    if (ReadDateString()) {
        if (IsReachTheEnd() || *input_ == '\t' || *input_ == '\r' || *input_ == '\n' ||
            *input_ == '#') {
            return true;
        }

        if (*input_ == 'T' || *input_ == ' ') {
            StringAddChar(*input_);
            input_++;
            remaining_input_--;
            return GetTimeImpl();
        }
    }
    return false;
}

bool Reader::ReadUTCOffsetString() {
    // HH:MM
    if (remaining_input_ < 5) return false;
    if (input_[2] == ':') {
        if (isdigit(input_[0]) && isdigit(input_[1]) && isdigit(input_[3]) && isdigit(input_[4])) {
            StringAddChar(reinterpret_cast<const char *>(input_), 5);
            int hh = BufferToInt(&input_[0], 2);
            int mm = BufferToInt(&input_[3], 2);
            dt_.SetGMTOffset(hh, mm);
            input_ += 5;
            remaining_input_ -= 5;
            return true;
        }
    }
    return false;
}

bool Reader::ReadTimeString() {
    // HH:MM:SS
    if (remaining_input_ < 8) return false;
    if (input_[2] == ':' && input_[5] == ':') {
        if (isdigit(input_[0]) && isdigit(input_[1]) && isdigit(input_[3]) && isdigit(input_[4]) &&
            isdigit(input_[6]) && isdigit(input_[7])) {
            StringAddChar(reinterpret_cast<const char *>(input_), 8);
            dt_.SetHour(BufferToInt(&input_[0], 2));
            dt_.SetMinute(BufferToInt(&input_[3], 2));
            dt_.SetSecond(BufferToInt(&input_[6], 2));
            input_ += 8;
            remaining_input_ -= 8;
            return true;
        }
    }
    return false;
}

bool Reader::ReadDateString() {
    // YYYY-MM-DD
    if (remaining_input_ < 10) return false;
    if (input_[4] == '-' && input_[7] == '-') {
        if (isdigit(input_[0]) && isdigit(input_[1]) && isdigit(input_[2]) && isdigit(input_[3]) &&
            isdigit(input_[5]) && isdigit(input_[6]) && isdigit(input_[8]) && isdigit(input_[9])) {
            StringAddChar(reinterpret_cast<const char *>(input_), 10);
            dt_.SetYear(BufferToInt(&input_[0], 4));
            dt_.SetMonth(BufferToInt(&input_[5], 2));
            dt_.SetDay(BufferToInt(&input_[8], 2));
            input_ += 10;
            remaining_input_ -= 10;
            return true;
        }
    }
    return false;
}

int Reader::BufferToInt(const uint8_t *buff, size_t len) {
    std::string str(reinterpret_cast<const char *>(buff), len);
    return atoi(str.c_str());
}
} // namespace internal
} // namespace TOML
