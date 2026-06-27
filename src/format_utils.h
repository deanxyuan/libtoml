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

#ifndef TOML_FORMAT_UTILS_H
#define TOML_FORMAT_UTILS_H

#include <string>

namespace TOML {

// Escape a string for TOML output (adds surrounding double quotes).
std::string escape_toml_string(const std::string& s);

// Escape a string for JSON output (adds surrounding double quotes).
std::string json_escape_string(const std::string& s);

// Escape a TOML key: returns bare key if valid [A-Za-z0-9-_], otherwise
// returns a quoted/escaped string.
std::string escape_toml_key(const std::string& key);

// Format a double for TOML output (inf/nan as bare words, decimal point
// required).
std::string format_float_toml(double v);

// Format a double for JSON output (inf/nan become "null").
std::string format_float_json(double v);

} // namespace TOML

#endif // TOML_FORMAT_UTILS_H
