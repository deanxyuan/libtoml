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

#ifndef TOML_SRC_FILE_READER_H_
#define TOML_SRC_FILE_READER_H_

#include <stdint.h>
#include <fstream>
#include <string>

namespace TOML {

class FileReader;
class LineIterator;

class FileLineInfo final {
    friend class FileReader;
    friend class LineIterator;

public:
    FileLineInfo(/* args */);
    ~FileLineInfo();

    uint32_t Line() const;
    const std::string &Data() const;

private:
    uint32_t line_;
    std::string data_;
};

class LineIterator {
    friend class FileReader;

public:
    LineIterator(/* args */);
    ~LineIterator();

    bool operator==(const LineIterator &oth);
    bool operator!=(const LineIterator &oth) { return !this->operator==(oth); }

    LineIterator &operator++();
    const LineIterator operator++(int);

    FileLineInfo &operator*();
    FileLineInfo *operator->();

private:
    FileReader *reader_;
    bool end_of_file_;
    FileLineInfo fline_;
};

class FileReader {
    friend class LineIterator;

public:
    explicit FileReader(const std::string &path);
    ~FileReader();

    LineIterator begin();
    LineIterator end();

private:
    bool NextLine(std::string &content, uint32_t &line);
    std::ifstream stream_;
};
} // namespace TOML
#endif // TOML_SRC_FILE_READER_H_
