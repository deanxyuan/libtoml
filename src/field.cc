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

#include "src/field.h"

#include <string.h>

namespace TOML {

Field::Field(/* args */) {}
Field::~Field() {}

void Field::set_key(const std::string &k) { key_ = k; }
const std::string &Field::key() const { return key_; }

intptr_t Field::value() const { return 0; }
void Field::set_value(intptr_t value) { (void)value; }
Type Field::type() const { return TOML_NULL; }

// ------------------------------

intptr_t StringField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void StringField::set_value(intptr_t value) { value_ = *reinterpret_cast<std::string *>(value); }
Type StringField::type() const { return TOML_STRING; }

// ------------------------------

intptr_t BooleanField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void BooleanField::set_value(intptr_t value) { value_ = *reinterpret_cast<bool *>(value); }
Type BooleanField::type() const { return TOML_BOOL; }

// ------------------------------

IntegerField::IntegerField() { memset(&value_, 0, sizeof(value_)); }
IntegerField::~IntegerField() {}
intptr_t IntegerField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void IntegerField::set_value(intptr_t value) { value_ = *reinterpret_cast<Number *>(value); }
Type IntegerField::type() const { return TOML_NUMBER; }

// ------------------------------

intptr_t DoubleField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void DoubleField::set_value(intptr_t value) { value_ = *reinterpret_cast<double *>(value); }
Type DoubleField::type() const { return TOML_DOUBLE; }

// ------------------------------

intptr_t ObjectField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void ObjectField::set_value(intptr_t value) { value_ = *reinterpret_cast<Object *>(value); }
Type ObjectField::type() const { return TOML_OBJECT; }

// ------------------------------

intptr_t ArrayField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void ArrayField::set_value(intptr_t value) { value_ = *reinterpret_cast<Array *>(value); }
Type ArrayField::type() const { return TOML_ARRAY; }

// ------------------------------

TimeField::TimeField() { memset(&value_, 0, sizeof(value_)); }
TimeField::~TimeField() {}
intptr_t TimeField::value() const { return reinterpret_cast<intptr_t>(&value_); }
void TimeField::set_value(intptr_t value) { value_ = *reinterpret_cast<Time *>(value); }
Type TimeField::type() const { return TOML_TIME; }

} // namespace TOML
