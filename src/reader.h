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
#include <set>
#include <stack>

#include "toml/impl/node.h"
#include "toml/impl/node_impl.h"

namespace TOML {
namespace internal {
class Reader {
public:
    static std::string Parse(const char *data, size_t len, Node *node);
    static constexpr uint32_t READ_CHAR_EOF = 0x7ffffff0;

    ~Reader();

    void Run();

    Status Result() const { return state_; }

private:
    Reader(const char *data, size_t len);

    inline size_t CurrentIndex() const { return input_ - original_input_ - 1; }
    inline bool IsReachTheEnd() { return (remaining_input_ == 0); }

    bool ParseComplexKey();
    bool CheckSeparator();
    bool ParseComplexValue();
    bool CheckTableTileRedefine();
    bool CheckArrayOfTableExists(const std::string &name);
    bool CheckArrayOfTablePathPrefix();
    std::string ComplexPathPrefix(const std::string &path);

    // TODO:
    // 当取完当前一组key=value数据，需要退栈
    // 退栈次数 complex_key_depth_
    bool UsingComplexKey();

    // 当遇到下一个表头时，需要退栈
    // 退栈次数 table_depth_
    bool UsingTableTitle();
    bool UsingTableTitleImpl();
    bool UsingArrayOfTableTitleImpl();

private:
    // ----获取字符串value ----
    bool GetStringValue();
    // 操作成功，结果存储在 strings_
    bool GetStringValueImpl();
    bool GetLiteralString();
    bool GetBasicString();
    bool GetMultiLineLiteralString();
    bool GetMultiLineBasicString();
    //--------------------------------

private:
    // 获取带正负号value（有正负数）
    bool GetNumberWithPrefix();
    bool GetNumberNoPrefix();
    bool GetDecimalNumber();
    bool GetFloatNumber();
    bool GetHexNumber();
    bool GetBinaryNumber();
    bool GetOctNumber();
    //--------------------------------

private:
    // 获取日期时间
    bool GetDateTime();
    bool GetTimeImpl();
    bool GetDateImpl();
    bool ReadTimeString();
    bool ReadDateString();
    bool ReadUTCOffsetString();

private:
    // 获取数组
    bool GetArrayImpl();

private:
    // 获取表
    bool GetInlineTableImpl();
    bool GetTitleOfTable();
    bool GetTitleOfTableImpl();

private:
    // 栈操作
    void UpdateNodeImpl(const Node &node);
    void PushStackImpl(const Node &node);
    bool PopStackImpl(Types type);
    int StackDepth();
    bool RestoreStack(int depth);
    bool ForceRestoreStack(int depth);

    // print
    void UpdateNode(const Node &node);
    void PushStack(const Node &node);
    bool PopStack(Types type);
    Node PushEmptyObject();
    Node PushEmptyArray();
    void PrintNode(const Node &node);

private:
    bool IsValidCharForRawKey(uint32_t c);
    bool IsValidCharForHex(uint32_t c);
    bool IsSpaceOrNextLine(uint32_t c);

    bool StartsWith(const char *prefix);
    void SetKey();
    void StringAddChar(uint32_t c);
    void StringAddPtr(const char *ptr);
    void StringAddPtr(const char *ptr, size_t count);
    void StringAddCharEx(int count, char ch);
    void StringAddString(const std::string &s);
    uint32_t LastInsertChar();

    // 从当前位置开始，测试连续有多少个相同的字符
    int TestSameCharCount(uint8_t ch);

    // 浮点，十进制，日期时间
    Types TestPossibleType();

    // 当前数据可能是浮点数，返回true
    bool TestNumberIsFloat();

    // 首字节必需为 #
    bool SkipComment();
    uint32_t SkipFrontSpace();

    double StringToDouble();
    int64_t StringToInt(int radix = 10);
    uint64_t StringToUInt(int radix = 10);

private:
    const uint8_t *original_input_;
    const uint8_t *input_;
    size_t remaining_input_;
    int table_depth_;
    int complex_key_depth_;
    bool is_table_title_;

    Status state_;
    DateTime::Detail dt_;

    std::string key_;
    std::string strings_;
    std::string error_;
    std::string desc_;
    std::string raw_path_; // 表头或表数组中的原始路径, 用于判断表头是否重复定义
    // for complex key or table title
    std::vector<std::string> path_; // 表头或表数组中的路径节点

    // key: PATH [[PATH]]  value: PATH 数组的最后的一个元素
    std::map<std::string, Node> array_of_table_table_;

    Node root_;
    std::stack<Node> stack_;
    // set of [table]
    std::set<std::string> table_title_set_;
};
} // namespace internal
} // namespace TOML
#endif // TOML_SRC_READER_H_
