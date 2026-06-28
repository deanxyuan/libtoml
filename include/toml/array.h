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

#ifndef TOML_ARRAY_H_
#define TOML_ARRAY_H_

#include <vector>
#include <string>
#include <stdexcept>

namespace toml {

class Value;

class Array {
public:
    Array() = default;
    Array(const Array&) = default;
    Array(Array&&) noexcept = default;
    Array& operator=(const Array&) = default;
    Array& operator=(Array&&) noexcept = default;

    void push_back(Value value);
    void insert(size_t index, Value value);

    Value& at(size_t index);
    const Value& at(size_t index) const;

    Value& operator[](size_t index);
    const Value& operator[](size_t index) const;

    Value& front();
    const Value& front() const;

    Value& back();
    const Value& back() const;

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    void clear();
    void erase(size_t index);
    void pop_back();

    Value* data() { return data_.data(); }
    const Value* data() const { return data_.data(); }

    using iterator = std::vector<Value>::iterator;
    using const_iterator = std::vector<Value>::const_iterator;

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    std::string to_toml() const;
    std::string to_json() const;

private:
    std::vector<Value> data_;
};

} // namespace toml

#endif // TOML_ARRAY_H_
