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

#include "toml/toml.h"

#include <stdint.h>

#include <atomic>

namespace TOML {
namespace internal {

class MemoryHookImpl : public MemoryHook {

public:
    MemoryHookImpl(/* args */) {}
    ~MemoryHookImpl() {}

    char *New(size_t bytes) override { return new char[bytes]; }

    char *NewAligned(size_t bytes) override {
        constexpr int alignment = (sizeof(void *) > 8) ? sizeof(void *) : 8;
        static_assert((alignment & (alignment - 1)) == 0, "Pointer size should be a power of 2");
        size_t needed = (bytes + alignment - 1) & (~(alignment - 1));
        return new char[needed];
    }

    void Delete(char *ptr) override { delete[] ptr; }

} default_memory_hook_impl__;

static std::atomic<MemoryHook *> global_memory_hook(&default_memory_hook_impl__);

MemoryHook *GetMemoryHook() { return global_memory_hook.load(std::memory_order_acquire); }
} // namespace internal

void InitMemoryHook(MemoryHook *hook) {
    internal::global_memory_hook.store((hook != nullptr) ? hook
                                                         : (&internal::default_memory_hook_impl__),
                                       std::memory_order_release);
}

} // namespace TOML
