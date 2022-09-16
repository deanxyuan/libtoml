#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include "node.h"

namespace TOML {
class TOMLReader {
public:
    static void LoadFromFile(const std::string &file);

    ~TOMLReader();

    void Run();

    Status Result() const { return state_; }

private:
    TOMLReader(const char *data, size_t len);

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
    void GetDecimalNumber(bool negative);
    void GetFloatNumber(bool negative);

    bool IsValidCharForRawKey(uint32_t c);

    bool StartsWith(const char *prefix);
    void SetKey();
    void StringAddChar(uint32_t c);
    void StringAddChar(const char *ptr);
    void StringAddChar(const char *ptr, int count);
    void StringAddChar(int count, char ch);

    void MoveForward(int offset);

    // 从当前位置开始，测试连续有多少个相同的字符
    int TestSameCharCount(uint8_t ch);
    int TestSameCharCountEx(uint8_t ch, uint8_t skip);

    // 当前数据可能是浮点数，返回true
    bool TestNumberIsFloat();

    void ShowKey();
    void ShowValue();
    void UpdateKeyValue();

private:
    const uint8_t *original_input_;
    const uint8_t *input_;
    size_t remaining_input_;

    Status state_;

    std::string key_;
    std::string strings_;
    std::string errors_;
    double d_value_;
    int64_t i_value_;
    uint64_t u_value_;

    Node root_;
    std::stack<Node> stack_;
};
} // namespace TOML
