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

#ifndef TOML_UTF8_H_
#define TOML_UTF8_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace TOML {
namespace utf8 {

// 将 Unicode 码点编码为 UTF-8，返回编码后的字节数
// 如果 buf 为 nullptr，只计算所需字节数
size_t encode(uint32_t codepoint, char* buf);

// 解码 UTF-8 序列，返回码点和消耗的字节数
// 如果无效，codepoint 为 0，consumed 为 0
struct DecodeResult {
    uint32_t codepoint;
    size_t consumed;
};
DecodeResult decode(const char* data, size_t len);

// 验证 UTF-8 字符串
bool validate(const char* data, size_t len);
bool validate(const std::string& str);

// 检查码点是否是有效的 Unicode 码点
bool is_valid_codepoint(uint32_t codepoint);

// 检查码点是否是标量值（非代理对）
bool is_scalar(uint32_t codepoint);

// 获取 UTF-8 序列长度（根据首字节）
size_t sequence_length(char first_byte);

// 将 \uXXXX 或 \UXXXXXXXX 转义序列转换为 UTF-8 字符串
// 返回 true 表示成功，result 包含 UTF-8 字节
// 返回 false 表示无效的码点
bool escape_to_utf8(uint32_t codepoint, std::string& result);

} // namespace utf8
} // namespace TOML

#endif // TOML_UTF8_H_
