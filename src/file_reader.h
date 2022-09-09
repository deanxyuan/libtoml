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

#include <stddef.h>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <string>

namespace TOML {

class ReaderService {

public:
    virtual ~ReaderService() {}
    virtual bool NextLine(std::string &data, uint32_t &line) = 0;
};

class ALineOfData final {
    friend class LineIterator;

public:
    ALineOfData(/* args */);
    ALineOfData(uint32_t line, const std::string &data);
    ~ALineOfData();

    uint32_t Line() const;
    const std::string &Data() const;

private:
    uint32_t line_;
    std::string data_;
};

class LineIterator {

public:
    LineIterator(/* args */);
    LineIterator(ReaderService *reader, bool eof = true);
    LineIterator(ReaderService *reader, const ALineOfData &data, bool eof = true);

    ~LineIterator();

    bool operator==(const LineIterator &oth);
    bool operator!=(const LineIterator &oth) { return !this->operator==(oth); }

    LineIterator &operator++();
    const LineIterator operator++(int);

    ALineOfData &operator*();
    ALineOfData *operator->();

private:
    ReaderService *reader_;
    bool end_of_file_;
    ALineOfData one_line_data_;
};

class FileReader final : public ReaderService {
    friend class LineIterator;

public:
    FileReader()  = default;
    ~FileReader() = default;

    FileReader(const FileReader &) = delete;
    FileReader &operator=(const FileReader &) = delete;

    bool open(const std::string &path);
    LineIterator begin();
    LineIterator end();

private:
    bool NextLine(std::string &data, uint32_t &line) override;
    std::ifstream stream_;
};

class DataReader final : public ReaderService {
    friend class LineIterator;

public:
    DataReader(const char *data, size_t len);
    ~DataReader() = default;

    DataReader(const DataReader &) = delete;
    DataReader &operator=(const DataReader &) = delete;

    LineIterator begin();
    LineIterator end();

private:
    bool NextLine(std::string &data, uint32_t &line) override;
    std::stringstream stream_;
};

} // namespace TOML
#endif // TOML_SRC_FILE_READER_H_
