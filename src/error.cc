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

#include "toml/value.h"
#include "toml/error.h"
#include <cstdio>

namespace TOML {

std::string Error::to_string() const {
    if (ok()) {
        return "";
    }
    char buf[256];
    if (location.line > 0) {
        std::snprintf(buf, sizeof(buf), "line %zu, column %zu: %s",
                       location.line, location.column, message.c_str());
    } else {
        std::snprintf(buf, sizeof(buf), "%s", message.c_str());
    }
    return buf;
}

} // namespace TOML
