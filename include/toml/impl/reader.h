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

#ifndef TOML_IMPL_READER_H_
#define TOML_IMPL_READER_H_

#include <stddef.h>
#include <string>

#include "toml/impl/node.h"
#include "toml/impl/node_impl.h"

namespace TOML {
/**
 * @brief  Load a TOML file.
 * @param file[in] Path to loaded TOML file
 * @param error[out] Error description, can be nullptr
 * @return return empty node if error occurs
 */
Node LoadFromFile(const std::string &file, std::string *error);

/**
 * @brief  Load the TOML string, which cannot contain the terminating null character.
 * @param buff[in] Pointer to a buffer where the TOML string is stored
 * @param len[in] Maximum number of bytes to be used in the buff
 * @param error[out] Error description, can be nullptr
 * @return return empty node if error occurs
 */
Node LoadFromString(const char *buff, size_t len, std::string *error);

} // namespace TOML
#endif // TOML_IMPL_READER_H_
