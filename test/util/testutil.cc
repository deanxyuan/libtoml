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

#include "testutil.h"
#include <vector>

namespace test {

namespace {
struct Test {
    const char* base;
    const char* name;
    void (*func)();
};

std::vector<Test>& GetTests() {
    static std::vector<Test> tests;
    return tests;
}

int g_failures = 0;
} // namespace

void RegisterFailure() {
    ++g_failures;
}

bool RegisterTest(const char* base, const char* name, void (*func)()) {
    Test t;
    t.base = base;
    t.name = name;
    t.func = func;
    GetTests().push_back(t);
    return true;
}

int RunAllTests() {
    int num = 0;
    g_failures = 0;
    const auto& tests = GetTests();
    for (size_t i = 0; i < tests.size(); i++) {
        const Test& t = tests[i];
        fprintf(stderr, "==== Test %s.%s\n", t.base, t.name);
        (*t.func)();
        ++num;
    }
    if (g_failures > 0) {
        fprintf(stderr, "==== FAILED %d assertions (ran %d tests)\n", g_failures, num);
        return 1;
    }
    fprintf(stderr, "==== PASSED %d tests\n", num);
    return 0;
}

} // namespace test
