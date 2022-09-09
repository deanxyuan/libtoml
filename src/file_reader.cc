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

#include "src/file_reader.h"

namespace TOML {

ALineOfData::ALineOfData(/* args */)
    : line_(0) {}

ALineOfData::ALineOfData(uint32_t line, const std::string &data)
    : line_(line)
    , data_(data) {}

ALineOfData::~ALineOfData() {}

uint32_t ALineOfData::Line() const { return line_; }

const std::string &ALineOfData::Data() const { return data_; }

LineIterator::LineIterator(/* args */)
    : reader_(nullptr)
    , end_of_file_(true) {}

LineIterator::LineIterator(ReaderService *reader, bool eof)
    : reader_(reader)
    , end_of_file_(eof) {}

LineIterator::LineIterator(ReaderService *reader, const ALineOfData &data, bool eof)
    : reader_(reader)
    , end_of_file_(eof)
    , one_line_data_(data) {}

LineIterator::~LineIterator() {}

bool LineIterator::operator==(const LineIterator &oth) {
    if (reader_ == oth.reader_ && end_of_file_ == oth.end_of_file_ &&
        one_line_data_.line_ == oth.one_line_data_.line_) {
        return true;
    }
    return false;
}

LineIterator &LineIterator::operator++() {
    if (reader_) {
        end_of_file_ = !reader_->NextLine(one_line_data_.data_, one_line_data_.line_);
    }
    return *this;
}

const LineIterator LineIterator::operator++(int) {
    LineIterator ret = *this;
    if (reader_) {
        end_of_file_ = !reader_->NextLine(one_line_data_.data_, one_line_data_.line_);
    }
    return ret;
}

ALineOfData &LineIterator::operator*() { return one_line_data_; }

ALineOfData *LineIterator::operator->() { return &one_line_data_; }

// ---------------------------------------------

bool FileReader::open(const std::string &path) {
    stream_.open(path);
    return stream_.is_open();
}

LineIterator FileReader::begin() {
    uint32_t line;
    std::string data;
    bool eof = !NextLine(data, line);
    return LineIterator(this, ALineOfData(line, data), eof);
}

LineIterator FileReader::end() { return LineIterator(this); }

bool FileReader::NextLine(std::string &data, uint32_t &number) {
    if (!stream_.is_open() || stream_.eof()) {
        data.clear();
        number = 0;
        return false;
    }

    number++;

    std::string line;
    std::getline(stream_, line);
    std::swap(data, line);

    return true;
}

DataReader::DataReader(const char *data, size_t len) { stream_ << std::string(data, len); }

LineIterator DataReader::begin() {
    uint32_t line;
    std::string data;
    bool eof = !NextLine(data, line);
    return LineIterator(this, ALineOfData(line, data), eof);
}

LineIterator DataReader::end() { return LineIterator(this); }

bool DataReader::NextLine(std::string &data, uint32_t &number) {
    if (stream_.eof()) {
        data.clear();
        number = 0;
        return false;
    }

    number++;

    std::string line;
    std::getline(stream_, line);
    std::swap(data, line);

    return true;
}
} // namespace TOML
