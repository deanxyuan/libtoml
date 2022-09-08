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

#ifndef TOML_TOML_H_
#define TOML_TOML_H_

#include <stddef.h>

#include <string>

namespace TOML {

enum Type {
    TOML_NULL,
    TOML_BOOL,
    TOML_NUMBER,
    TOML_DOUBLE,
    TOML_STRING,
    TOML_OBJECT,
    TOML_ARRAY,
    TOML_TIME, // Time or Date or DateTime
};

class MemoryHook {

public:
    virtual ~MemoryHook() {}

    virtual char *New(size_t bytes) = 0;

    virtual char *NewAligned(size_t bytes) = 0;

    virtual void Delete(char *ptr) = 0;
};

// Supply new[bytes], new[aligned size] and delete functions to libtoml
void InitMemoryHook(MemoryHook *hook);

// Parsing TOML format data
bool Parse(const char *data, size_t len, std::string *error);

// Parsing TOML file
bool ParseFromFile(const std::string &file_name, std::string *error);

} // namespace TOML

#endif // TOML_TOML_H_
