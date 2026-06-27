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

#ifndef TOML_WRITER_H_
#define TOML_WRITER_H_

#include "toml/value.h"
#include <string>

namespace toml {

std::string to_toml(const Value& value);
std::string to_toml(const Table& table);

struct FormatOptions {
    size_t indent;
    char indent_char;
    bool sorted_keys;
    FormatOptions() : indent(4), indent_char(' '), sorted_keys(false) {}
};

std::string to_toml(const Value& value, const FormatOptions& opts);
std::string to_json(const Value& value);

} // namespace toml

#endif // TOML_WRITER_H_
