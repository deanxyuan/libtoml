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

#include <string>
#include <vector>

namespace TOML {

/**
 * @brief Convert a UCS char to utf8 string
 * @param code The ucs code
 * @param output The result string
 * @return Return operation result
 */
bool UCSToUTF8(int64_t code, std::string *output);

/**
 * @brief Determine if the byte is a valid TOML STRING
 * @param multiline is it multi-line text ?
 * @param ch Byte to be judged
 * @return Return TRUE if allowed, otherwise return FALSE
 * @Note: U+0000 to U+0008, U+000A to U+001F, U+007F must be escaped
 */
bool IsValidCharForString(bool multiline, uint8_t ch);

/**
 * @brief Whether CC exists in Target String
 * @param target The target string to be searched
 * @param cc The byte for comparison
 * @return Exists returns TRUE, otherwise returns FALSE
 */
bool IsByteExistsInTarget(const char *target, uint8_t cc);

int64_t StringToInt(const std::string &str, int radix = 10);
uint64_t StringToUInt(const std::string &str, int radix = 10);
double StringToFloat(const std::string &str);
int BufferToInt(const uint8_t *buff, size_t len);
int BufferToInt(const char *buff, int len);

std::string ComplexPathPrefix(const std::vector<std::string> &vec);
std::string GetVectorLastElement(const std::vector<std::string> &vec);

bool IsValidCharForRawKey(uint32_t c);
bool IsSpaceOrNextLine(uint32_t c);
bool IsCharExists(const std::string &target, uint8_t cc);
bool IsCharExists(const std::string &target, const std::string &sub);
} // namespace TOML
#endif // TOML_SRC_COMMON_H_
