/*
 *
 * Copyright 2022-2026 libtoml authors.
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

#ifndef TOML_TESTUTIL_H_
#define TOML_TESTUTIL_H_

#include "toml/toml.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace test {

// Run all unit tests registered by the TEST macro.
// Returns 0 if all tests pass, non-zero if any test failed.
int RunAllTests();

// Record a test failure (called by Tester destructor).
void RegisterFailure();

// An instance of Tester is allocated to hold temporary state during
// the execution of an assertion.
class Tester {
private:
    bool ok_;
    const char* fname_;
    int line_;
    std::stringstream ss_;

public:
    Tester(const char* f, int l)
        : ok_(true)
        , fname_(f)
        , line_(l) {}

    Tester(const Tester&) = delete;
    Tester& operator=(const Tester&) = delete;

    ~Tester() {
        if (!ok_) {
            fprintf(stderr, "%s:%d:%s\n", fname_, line_, ss_.str().c_str());
            RegisterFailure();
        }
    }

    Tester& Is(bool b, const char* msg) {
        if (!b) {
            ss_ << " Assertion failure: " << msg;
            ok_ = false;
        }
        return *this;
    }

    Tester& FloatEq(double x, double y, double eps = 1e-10) {
        if (std::fabs(x - y) > eps) {
            ss_ << " failed: " << x << " != " << y << " (diff=" << std::fabs(x - y) << ")";
            ok_ = false;
        }
        return *this;
    }

    // Attach the specified value to the error message if an error has occurred
    template <class V>
    Tester& operator<<(const V& value) {
        if (!ok_) {
            ss_ << " " << value;
        }
        return *this;
    }
};

#define ASSERT_TRUE(c)   ::test::Tester(__FILE__, __LINE__).Is((c), #c)
#define ASSERT_FALSE(c)  ::test::Tester(__FILE__, __LINE__).Is(!(c), "!(" #c ")")
#define ASSERT_EQ(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) == (b)), #a " == " #b)
#define ASSERT_NE(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) != (b)), #a " != " #b)
#define ASSERT_GE(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) >= (b)), #a " >= " #b)
#define ASSERT_GT(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) > (b)), #a " > " #b)
#define ASSERT_LE(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) <= (b)), #a " <= " #b)
#define ASSERT_LT(a, b)  ::test::Tester(__FILE__, __LINE__).Is(((a) < (b)), #a " < " #b)
#define ASSERT_FLOAT_EQ(a, b) ::test::Tester(__FILE__, __LINE__).FloatEq((a), (b))

#define ASSERT_THROW(expr, ExType)                                                                 \
    do {                                                                                           \
        bool caught_ = false;                                                                      \
        try {                                                                                      \
            expr;                                                                                  \
        } catch (const ExType&) {                                                                  \
            caught_ = true;                                                                        \
        } catch (...) {}                                                                           \
        ::test::Tester(__FILE__, __LINE__).Is(caught_, "expected " #ExType " from " #expr);         \
    } while (0)

#define ASSERT_NO_THROW(expr)                                                                      \
    do {                                                                                           \
        ::test::Tester _tester_(__FILE__, __LINE__);                                               \
        try {                                                                                      \
            expr;                                                                                  \
        } catch (const std::exception&) {                                                          \
            _tester_.Is(false, "unexpected exception from " #expr);                                \
        } catch (...) {                                                                            \
            _tester_.Is(false, "unexpected unknown exception from " #expr);                        \
        }                                                                                          \
    } while (0)

#define TCONCAT(a, b)  TCONCAT1(a, b)
#define TCONCAT1(a, b) a##b

// Register the specified test. Typically not used directly, but
// invoked via the macro expansion of TEST.
bool RegisterTest(const char* base, const char* name, void (*func)());

/**
 * example:
 *   TEST(Array, BasicTest)
 *   {
 *       ASSERT_TRUE(...);
 *   }
 */
#define TEST(TestSuit, TestName)                                                                   \
    class TCONCAT(_Test_, __LINE__) {                                                              \
    public:                                                                                        \
        void _Run();                                                                               \
        static void _RunIt() {                                                                     \
            TCONCAT(_Test_, __LINE__) t;                                                           \
            t._Run();                                                                              \
        }                                                                                          \
    };                                                                                             \
    bool TCONCAT(_Test_ignored_, __LINE__) =                                                       \
        ::test::RegisterTest(#TestSuit, #TestName, &TCONCAT(_Test_, __LINE__)::_RunIt);            \
    void TCONCAT(_Test_, __LINE__)::_Run()

#define RUN_ALL_TESTS()                                                                            \
    int main(int argc, char* argv[]) {                                                             \
        (void)argc;                                                                                \
        (void)argv;                                                                                \
        return ::test::RunAllTests();                                                              \
    }

// ============= Helper functions =============

// Check if Table contains a string value at key (returns bool)
inline bool CheckTableHasStringValue(const toml::Table& table,
                                     const std::string& key,
                                     const std::string& expected) {
    if (!table.contains(key)) return false;
    const auto& val = table.at(key);
    return val.is_string() && val.as_string() == expected;
}

// Check if Table contains a string value at key (void, with diagnostics)
inline void CheckTableString(const toml::Table& table,
                             const std::string& key,
                             const std::string& expected) {
    if (!table.contains(key)) {
        fprintf(stderr, "  key '%s' not found in table\n", key.c_str());
        RegisterFailure();
        return;
    }
    const auto& val = table.at(key);
    if (!val.is_string()) {
        fprintf(stderr, "  key '%s': expected string, got %s\n",
                key.c_str(), toml::type_name(val.type()));
        RegisterFailure();
        return;
    }
    if (val.as_string() != expected) {
        fprintf(stderr, "  key '%s': expected \"%s\", got \"%s\"\n",
                key.c_str(), expected.c_str(), val.as_string().c_str());
        RegisterFailure();
        return;
    }
}

// Check if Table contains an integer value at key
inline bool CheckTableHasIntValue(const toml::Table& table,
                                  const std::string& key,
                                  int64_t expected) {
    if (!table.contains(key)) return false;
    const auto& val = table.at(key);
    return val.is_integer() && val.as_integer() == expected;
}

// Check if Table contains a boolean value at key
inline bool CheckTableHasBoolValue(const toml::Table& table,
                                   const std::string& key,
                                   bool expected) {
    if (!table.contains(key)) return false;
    const auto& val = table.at(key);
    return val.is_boolean() && val.as_bool() == expected;
}

// Check if Table contains a float value at key
inline bool CheckTableHasFloatValue(const toml::Table& table,
                                    const std::string& key,
                                    double expected) {
    if (!table.contains(key)) return false;
    const auto& val = table.at(key);
    return val.is_float() && std::fabs(val.as_float() - expected) < 1e-10;
}

// Check if Array contains a string value at index
inline bool CheckArrayHasStringValue(const toml::Array& arr,
                                     size_t index,
                                     const std::string& expected) {
    if (index >= arr.size()) return false;
    const auto& val = arr.at(index);
    return val.is_string() && val.as_string() == expected;
}

// Check if Array contains an integer value at index
inline bool CheckArrayHasIntValue(const toml::Array& arr,
                                  size_t index,
                                  int64_t expected) {
    if (index >= arr.size()) return false;
    const auto& val = arr.at(index);
    return val.is_integer() && val.as_integer() == expected;
}

// Parse string and abort on failure
inline toml::Value ParseOrFail(const std::string& str) {
    auto result = toml::parse_string(str);
    if (!result.ok()) {
        fprintf(stderr, "Parse failed: %s\n", result.error.to_string().c_str());
        exit(1);
    }
    return result.value;
}

// Parse file and abort on failure
inline toml::Value ParseFileOrFail(const std::string& path) {
    auto result = toml::parse_file(path);
    if (!result.ok()) {
        fprintf(stderr, "Parse file failed: %s\n", result.error.to_string().c_str());
        exit(1);
    }
    return result.value;
}

// Parse string and expect failure
inline toml::Error ParseExpectFail(const std::string& str) {
    auto result = toml::parse_string(str);
    if (result.ok()) {
        fprintf(stderr, "Expected parse to fail but it succeeded\n");
        exit(1);
    }
    return result.error;
}

} // namespace test

#endif // TOML_TESTUTIL_H_
