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
struct ComplexPathRecord {
    // 去掉空格后的表头，形如 A.B.C
    std::string title;
    // 表头路径 title_path[0]=A, title_path[1]=B, title_path[2]=C
    std::vector<std::string> title_path;

    // 去掉空格后的复合键，形如 A.B.C
    std::string key;
    // 表头路径 key_path[0]=A, key_path[1]=B, key_path[2]=C
    std::vector<std::string> key_path;
};

class Reader {
public:
    static std::string Parse(const char *data, size_t len, Node *node);
    static constexpr uint32_t READ_CHAR_EOF = 0x7ffffff0;
    static constexpr int ALLOWED_REPEATED   = 1;
    static constexpr int DISABLE_REPEATED   = 2;

    ~Reader();

    void Run();

    Status Result() const { return state_; }

private:
    Reader(const char *data, size_t len);

    inline size_t CurrentIndex() const { return input_ - original_input_ - 1; }
    inline bool IsReachTheEnd() { return (remaining_input_ == 0); }

    bool CheckSeparator();
    bool CheckTableTileRedefine();
    void DisablePrevTable();
    bool ParseComplexKey();
    bool ParseComplexValue();

    // 当取完当前一组key=value数据，需要退栈
    // 退栈次数 complex_key_depth_
    bool UsingComplexKey();

    // 当遇到下一个表头时，需要退栈
    // 退栈次数 table_depth_
    bool UsingTableTitle();
    bool UsingTableTitleImpl();
    bool UsingArrayOfTableTitleImpl();

    // 首字节必需为 #
    bool SkipComment();
    // 返回EOF或前一个字节
    uint32_t SkipFrontSpace();

private:
    // ----获取字符串value ----
    bool GetStringValue();
    // 操作结果存储在 strings_
    bool GetStringValueImpl();
    bool GetLiteralString();
    bool GetBasicString();
    bool GetMultiLineLiteralString();
    bool GetMultiLineBasicString();

private:
    // 获取带正负号value（有正负数）
    bool GetNumberWithPrefix();
    bool GetNumberNoPrefix();
    bool GetDecimalNumber();
    bool GetFloatNumber();
    bool GetHexNumber();
    bool GetBinaryNumber();
    bool GetOctNumber();

private:
    // 获取日期时间
    bool GetDateTime();
    bool GetTimeImpl();
    bool GetDateImpl();
    bool ReadTimeString();
    bool ReadDateString();
    bool ReadUTCOffsetString();

private:
    // array, table, array of table
    bool GetArrayImpl();
    bool GetInlineTableImpl();
    bool GetTitleOfTable();
    bool GetTitleOfTableImpl();

private:
    // 栈操作
    int StackDepth();
    bool RestoreStack(int depth);
    bool ForceRestoreStack(int depth);
    void UpdateNode(const Node &node);
    void PushStack(const Node &node);
    bool PopStack(Types type);
    bool StaticArrayPop();
    bool InlinedTablePop();
    Node PushEmptyTable();
    Node PushEmptyArray();

private:
    // string_ or input_
    uint32_t LastInsertChar();

    bool StartsWith(const char *prefix);
    void SetKey();
    void StringAddChar(uint32_t c);
    void StringAddCString(const char *ptr);
    void StringAddCString(const char *ptr, size_t count);
    void StringAddCharEx(int count, char ch);
    void StringAddString(const std::string &s);

    // 测试浮点、十进制整数或日期时间
    Types TestPossibleType();
    bool TestNumberIsFloat();
    // 从当前位置开始，测试连续有多少个相同的字符
    int TestSameCharCount(uint8_t ch);

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

    ComplexPathRecord prev_;
    ComplexPathRecord current_;

    // key: PATH [[PATH]]  value: PATH 数组的最后的一个元素
    std::map<std::string, Node> array_of_table_map_;

    Node root_;
    std::stack<Node> stack_;
    // key:full-path, value: flag
    // 除表数组之外的全部 table 的全局路径表
    // flag:0 表示允许重复, 为1表示不允许重复
    std::map<std::string, int> table_map_;
};
} // namespace internal
} // namespace TOML
#endif // TOML_SRC_READER_H_
