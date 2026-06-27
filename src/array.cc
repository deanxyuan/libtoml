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

#include "toml/array.h"
#include "toml/value.h"

namespace TOML {

void Array::push_back(Value value) {
    data_.push_back(std::move(value));
}

void Array::insert(size_t index, Value value) {
    if (index > data_.size()) {
        throw std::out_of_range("Array insert index out of range");
    }
    data_.insert(data_.begin() + index, std::move(value));
}

Value& Array::at(size_t index) {
    if (index >= data_.size()) {
        throw std::out_of_range("Array index out of range");
    }
    return data_[index];
}

const Value& Array::at(size_t index) const {
    if (index >= data_.size()) {
        throw std::out_of_range("Array index out of range");
    }
    return data_[index];
}

Value& Array::operator[](size_t index) {
    return data_[index];
}

const Value& Array::operator[](size_t index) const {
    return data_[index];
}

Value& Array::front() {
    return data_.front();
}

const Value& Array::front() const {
    return data_.front();
}

Value& Array::back() {
    return data_.back();
}

const Value& Array::back() const {
    return data_.back();
}

void Array::clear() {
    data_.clear();
}

void Array::erase(size_t index) {
    if (index >= data_.size()) {
        throw std::out_of_range("Array erase index out of range");
    }
    data_.erase(data_.begin() + index);
}

void Array::pop_back() {
    data_.pop_back();
}

std::string Array::to_toml() const {
    std::string result = "[";
    for (size_t i = 0; i < data_.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += data_[i].to_toml();
    }
    result += "]";
    return result;
}

std::string Array::to_json() const {
    std::string result = "[";
    for (size_t i = 0; i < data_.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += data_[i].to_json();
    }
    result += "]";
    return result;
}

} // namespace TOML
