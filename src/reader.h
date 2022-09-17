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

#ifndef TOML_SRC_READER_H_
#define TOML_SRC_READER_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include "toml/impl/node.h"

namespace TOML {
namespace internal {
class Reader {
public:
    static std::string Parse(const char *data, size_t len, Node *node);

    ~Reader();

    void Run();

    Status Result() const { return state_; }

private:
    Reader(const char *data, size_t len);

    inline size_t CurrentIndex() const { return input_ - original_input_ - 1; }
    inline bool IsReachTheEnd() { return (remaining_input_ == 0); }

    bool GetRawKeyImpl();
    bool GetKeyImpl(uint8_t flag);
    bool CheckSeparator();
    bool GetValueImpl();

    // ----获取字符串value ----
    bool GetStringValueImpl();
    void GetLiteralString();
    void GetBasicString();
    void GetMultiLineLiteralString();
    void GetMultiLineBasicString();
    //--------------------------------

    // 获取带正负号value（有正负数）
    void GetNumberWithPrefix();
    void GetNumberValueImpl();
    bool GetDecimalNumber();
    bool GetFloatNumber();
    bool GetHexNumber();
    bool GetBinaryNumber();
    bool GetOctNumber();
    //--------------------------------

    // 获取日期时间
    bool GetDateTimeImpl();

    bool IsValidCharForRawKey(uint32_t c);
    bool IsValidCharForHex(uint32_t c);
    bool IsSpaceOrNextLine(uint32_t c);

    bool StartsWith(const char *prefix);
    void SetKey();
    void StringAddChar(uint32_t c);
    void StringAddChar(const char *ptr);
    void StringAddChar(const char *ptr, int count);
    void StringAddChar(int count, char ch);
    void StringAddChar(const std::string &s);

    void MoveForward(int offset);

    // 从当前位置开始，测试连续有多少个相同的字符
    int TestSameCharCount(uint8_t ch);

    // 浮点，十进制，日期时间
    Types TestPossibleType();

    // 当前数据可能是浮点数，返回true
    bool TestNumberIsFloat();

    void UpdateKeyValue();
    void UpdateNode(Node node);

    double StringToDouble();
    int64_t StringToInt(int radix = 10);
    uint64_t StringToUInt(int radix = 10);

private:
    const uint8_t *original_input_;
    const uint8_t *input_;
    size_t remaining_input_;

    Status state_;

    std::string key_;
    std::string strings_;
    std::string error_;

    Node root_;
    std::stack<Node> stack_;
};
} // namespace internal
} // namespace TOML
#endif // TOML_SRC_READER_H_
