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

FileLineInfo::FileLineInfo(/* args */)
    : line_(0) {}

FileLineInfo::~FileLineInfo() {}

uint32_t FileLineInfo::Line() const { return line_; }

const std::string &FileLineInfo::Data() const { return data_; }

LineIterator::LineIterator(/* args */)
    : reader_(nullptr)
    , end_of_file_(true) {}

LineIterator::~LineIterator() {}

bool LineIterator::operator==(const LineIterator &oth) {
    if (reader_ == oth.reader_ && end_of_file_ == oth.end_of_file_ &&
        fline_.line_ == oth.fline_.line_) {
        return true;
    }
    return false;
}

LineIterator &LineIterator::operator++() {
    if (reader_) {
        end_of_file_ = !reader_->NextLine(fline_.data_, fline_.line_);
    }
    return *this;
}

const LineIterator LineIterator::operator++(int) {
    LineIterator ret = *this;
    if (reader_) {
        end_of_file_ = !reader_->NextLine(fline_.data_, fline_.line_);
    }
    return ret;
}

FileLineInfo &LineIterator::operator*() { return fline_; }

FileLineInfo *LineIterator::operator->() { return &fline_; }

FileReader::FileReader(const std::string &path)
    : stream_(path) {}

FileReader::~FileReader() {}

LineIterator FileReader::begin() {
    LineIterator obj;
    obj.reader_      = this;
    obj.end_of_file_ = !NextLine(obj.fline_.data_, obj.fline_.line_);
    return obj;
}

LineIterator FileReader::end() {
    LineIterator obj;
    obj.reader_ = this;
    return obj;
}
bool FileReader::NextLine(std::string &content, uint32_t &number) {
    if (!stream_.is_open() || stream_.eof()) {
        content.clear();
        number = 0;
        return false;
    }

    number++;

    std::string line;
    std::getline(stream_, line);
    std::swap(content, line);

    return true;
}
} // namespace TOML
