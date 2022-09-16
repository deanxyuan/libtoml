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

#ifndef TOML_SRC_COMMON_H_
#define TOML_SRC_COMMON_H_

#include <stddef.h>
#include <stdint.h>

namespace TOML {

/**
 *	Convert a UCS char to utf8 code, and return it in buf.
 *	Return #bytes used in buf to encode the char, or
 *	-1 on error.
 */
int toml_ucs_to_utf8(int64_t code, char buf[6]);

/**
 * U+0000 to U+0008, U+000A to U+001F, U+007F must be escaped
 */
bool toml_is_valid_char(bool multi_line, uint8_t ch);

// 判断字节 cc 是否在 target 中存在
// 存在返回true, 不存在返回 false
bool IsByteExists(const char *target, uint8_t cc);

/**
 * @brief 相关搜索函数
 * @param buff 待搜索的地址
 * @param ch 字符
 * @param count 待搜索的空间长度
 * @return 返回与初始位置的偏移，找不到则返回-1
 */
int FindNextChar(const uint8_t *buff, int ch, size_t count);
} // namespace TOML
#endif // TOML_SRC_COMMON_H_
