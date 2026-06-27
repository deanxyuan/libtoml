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

#ifndef TOML_TABLE_H_
#define TOML_TABLE_H_

#include <string>
#include <unordered_map>
#include <stdexcept>

namespace toml {

class Value;

class Table {
public:
    using iterator = std::unordered_map<std::string, Value>::iterator;
    using const_iterator = std::unordered_map<std::string, Value>::const_iterator;

    Table() = default;
    Table(const Table&) = default;
    Table(Table&&) noexcept = default;
    Table& operator=(const Table&) = default;
    Table& operator=(Table&&) noexcept = default;

    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;

    Value& at(const std::string& key);
    const Value& at(const std::string& key) const;

    bool contains(const std::string& key) const;
    size_t count(const std::string& key) const;

    std::pair<iterator, bool> insert(const std::string& key, Value value);
    std::pair<iterator, bool> insert_or_assign(const std::string& key, Value value);

    size_t erase(const std::string& key);
    void clear();

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    iterator find(const std::string& key);
    const_iterator find(const std::string& key) const;

    std::string to_toml() const;
    std::string to_json() const;

    // Inline tables (defined with {}) cannot be extended after definition.
    bool is_inline() const { return is_inline_; }
    void set_inline(bool v) { is_inline_ = v; }

    // Tables implicitly created by dotted keys cannot be redefined by
    // a [table] header (but can be navigated through for sub-tables).
    bool is_from_dotted_key() const { return from_dotted_key_; }
    void set_from_dotted_key(bool v) { from_dotted_key_ = v; }

private:
    std::unordered_map<std::string, Value> data_;
    bool is_inline_ = false;
    bool from_dotted_key_ = false;
};

} // namespace toml

#endif // TOML_TABLE_H_
