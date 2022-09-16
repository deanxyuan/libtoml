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

#ifndef TOML_IMPL_TYPES_H_
#define TOML_IMPL_TYPES_H_

namespace TOML {

enum Status {
    PARSE_STATUS_NULL,
    PARSE_STATUS_ERROR,
    PARSE_STATUS_SUCCESS,
};

enum class Types : int {
    TOML_NULL,
    TOML_BOOLEAN,
    TOML_STRING,
    TOML_INTEGER,
    TOML_DOUBLE,
    TOML_DATETIME,
    TOML_OBJECT,
    TOML_ARRAY,
};
constexpr Types kNull     = Types::TOML_NULL;
constexpr Types kBoolean  = Types::TOML_BOOLEAN;
constexpr Types kString   = Types::TOML_STRING;
constexpr Types kInteger  = Types::TOML_INTEGER;
constexpr Types kDouble   = Types::TOML_DOUBLE;
constexpr Types kDateTime = Types::TOML_DATETIME;
constexpr Types kObject   = Types::TOML_OBJECT;
constexpr Types kArray    = Types::TOML_ARRAY;

} // namespace TOML

#endif // TOML_IMPL_TYPES_H_
