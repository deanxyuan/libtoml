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
#include "src/common.h"

namespace TOML {
namespace internal {
bool Reader::GetDateTime() {
    // YYYY-MM-DD or HH:MM:SS
    if (remaining_input_ < 8) return false;
    DateTime::InitDetail(&dt_);
    if (input_[2] == ':') {
        return GetTimeImpl();
    } else if (input_[4] == '-') {
        return GetDateImpl();
    }
    return false;
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
        dt_.SetSpecific(true);
        return true;
    }

    if (c == '.') {
        // read microseconds
        StringAddChar(c);

        char buff[12];
        memset(buff, '0', sizeof(buff));

        size_t i = 0;
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
            dt_.SetMicroSecond(BufferToInt(buff, 3) * 1000);
        } else if (i <= 6) {
            // microseconds
            dt_.SetMicroSecond(BufferToInt(buff, 6));
        } else {
            // nanoseconds
            dt_.SetMicroSecond(BufferToInt(buff, 9) / 1000);
        }

        StringAddPtr(buff, i);
        input_ += i;
        remaining_input_ -= i;

        if (IsReachTheEnd()) {
            return true;
        }
        c = *input_++;
        remaining_input_--;
    }

    if (c == '+' || c == '-') {
        StringAddChar(c);
        return ReadUTCOffsetString();
    }
    return true;
}
bool Reader::GetDateImpl() {
    if (ReadDateString()) {
        if (IsReachTheEnd()) {
            return true;
        }

        if (*input_ == 'T') {
            StringAddChar(*input_);
            input_++;
            remaining_input_--;
            return GetTimeImpl();
        }
        if (*input_ == ' ') {
            // 尝试加载时间
            if (remaining_input_ > 1 && input_[1] >= '0' && input_[1] <= '9') {
                StringAddChar(*input_);
                input_++;
                remaining_input_--;
                return GetTimeImpl();
            }
        }
        return true;
    }
    return false;
}

bool Reader::ReadUTCOffsetString() {
    // HH:MM
    if (remaining_input_ < 5) return false;
    if (input_[2] == ':') {
        if (isdigit(input_[0]) && isdigit(input_[1]) && isdigit(input_[3]) && isdigit(input_[4])) {
            StringAddPtr(reinterpret_cast<const char *>(input_), 5);
            int hh = BufferToInt(&input_[0], 2);
            int mm = BufferToInt(&input_[3], 2);
            dt_.SetGMTOffset(hh, mm);
            dt_.SetSpecific(true);
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
            StringAddPtr(reinterpret_cast<const char *>(input_), 8);
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
            StringAddPtr(reinterpret_cast<const char *>(input_), 10);
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

} // namespace internal
} // namespace TOML
