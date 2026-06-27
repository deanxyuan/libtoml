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

#include "toml/table.h"
#include "toml/value.h"

namespace TOML {

Value& Table::operator[](const std::string& key) {
    return data_[key];
}

const Value& Table::operator[](const std::string& key) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        throw std::out_of_range("Key not found: " + key);
    }
    return it->second;
}

Value& Table::at(const std::string& key) {
    auto it = data_.find(key);
    if (it == data_.end()) {
        throw std::out_of_range("Key not found: " + key);
    }
    return it->second;
}

const Value& Table::at(const std::string& key) const {
    auto it = data_.find(key);
    if (it == data_.end()) {
        throw std::out_of_range("Key not found: " + key);
    }
    return it->second;
}

bool Table::contains(const std::string& key) const {
    return data_.find(key) != data_.end();
}

size_t Table::count(const std::string& key) const {
    return data_.count(key);
}

std::pair<Table::iterator, bool> Table::insert(const std::string& key, Value value) {
    return data_.insert({key, std::move(value)});
}

std::pair<Table::iterator, bool> Table::insert_or_assign(const std::string& key, Value value) {
    return data_.insert_or_assign(key, std::move(value));
}

size_t Table::erase(const std::string& key) {
    return data_.erase(key);
}

void Table::clear() {
    data_.clear();
}

Table::iterator Table::find(const std::string& key) {
    return data_.find(key);
}

Table::const_iterator Table::find(const std::string& key) const {
    return data_.find(key);
}

std::string Table::to_toml() const {
    std::string result;
    for (auto it = data_.begin(); it != data_.end(); ++it) {
        if (it != data_.begin()) {
            result += '\n';
        }
        result += it->first;
        result += " = ";
        result += it->second.to_toml();
    }
    return result;
}

std::string Table::to_json() const {
    std::string result = "{";
    bool first = true;
    for (const auto& pair : data_) {
        if (!first) {
            result += ", ";
        }
        first = false;
        // JSON key must be a quoted string
        result += '"';
        result += pair.first;
        result += "\": ";
        result += pair.second.to_json();
    }
    result += '}';
    return result;
}

} // namespace TOML
