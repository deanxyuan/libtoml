/*
 *
 * Copyright 2022-2026 libtoml authors.
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

#include "toml/toml.h"
#include "util/testutil.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

TEST(Table, tab01) {
    std::string path = TEST_CASE_DIR "/tab01.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;
    const auto& table = node.as_table().at("table-1");
    ASSERT_EQ(table.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(table.as_table(), "key1", "some string"));
    ASSERT_TRUE(test::CheckTableHasIntValue(table.as_table(), "key2", 123));

    const auto& n2 = node.as_table().at("table-2");
    ASSERT_EQ(n2.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(n2.as_table(), "key1", "another string"));
    ASSERT_TRUE(test::CheckTableHasIntValue(n2.as_table(), "key2", 456));
}

TEST(Table, tab02) {
    std::string path = TEST_CASE_DIR "/tab02.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    /*
        [dog."tater.man"]
        type.name = "pug"
    */
    const auto& dog = node.as_table().at("dog");
    ASSERT_EQ(dog.as_table().size(), 1);
    const auto& man = dog.as_table().at("tater.man");
    ASSERT_EQ(man.as_table().size(), 1);
    const auto& type = man.as_table().at("type");
    ASSERT_EQ(type.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasStringValue(type.as_table(), "name", "pug"));
}

TEST(Table, tab03) {
    std::string path = TEST_CASE_DIR "/tab03.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    /*
        [a.b.c]
    */
    const auto& a = node.as_table().at("a");
    ASSERT_EQ(a.as_table().size(), 1);
    const auto& b = a.as_table().at("b");
    ASSERT_EQ(b.as_table().size(), 1);
    const auto& c = b.as_table().at("c");
    ASSERT_EQ(c.as_table().size(), 0);

    const auto& d = node.as_table().at("d");
    ASSERT_EQ(d.as_table().size(), 1);
    const auto& e = d.as_table().at("e");
    ASSERT_EQ(e.as_table().size(), 1);
    const auto& f = e.as_table().at("f");
    ASSERT_EQ(f.as_table().size(), 0);

    const auto& g = node.as_table().at("g");
    ASSERT_EQ(g.as_table().size(), 1);
    const auto& h = g.as_table().at("h");
    ASSERT_EQ(h.as_table().size(), 1);
    const auto& i = h.as_table().at("i");
    ASSERT_EQ(i.as_table().size(), 0);

    const auto& j = node.as_table().at("j");
    ASSERT_EQ(j.as_table().size(), 1);
    const auto& k = j.as_table().at("ʞ");
    ASSERT_EQ(k.as_table().size(), 1);
    const auto& l = k.as_table().at("l");
    ASSERT_EQ(l.as_table().size(), 0);
}

TEST(Table, tab04) {
    std::string path = TEST_CASE_DIR "/tab04.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& x = node.as_table().at("x");
    ASSERT_EQ(x.as_table().size(), 1);
    const auto& y = x.as_table().at("y");
    ASSERT_EQ(y.as_table().size(), 1);
    const auto& z = y.as_table().at("z");
    ASSERT_EQ(z.as_table().size(), 1);
    const auto& w = z.as_table().at("w");
    ASSERT_EQ(w.as_table().size(), 0);
}

TEST(Table, tab05) {
    std::string path = TEST_CASE_DIR "/tab05.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Table, tab06) {
    std::string path = TEST_CASE_DIR "/tab06.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Table, tab07) {
    std::string path = TEST_CASE_DIR "/tab07.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& fruit = node.as_table().at("fruit");
    ASSERT_EQ(fruit.as_table().size(), 2);
    const auto& y = fruit.as_table().at("apple");
    ASSERT_EQ(y.as_table().size(), 0);
    const auto& z = fruit.as_table().at("orange");
    ASSERT_EQ(z.as_table().size(), 0);
    const auto& w = node.as_table().at("animal");
    ASSERT_EQ(w.as_table().size(), 0);
}

TEST(Table, tab08) {
    std::string path = TEST_CASE_DIR "/tab08.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& n1 = node.as_table().at("name");
    ASSERT_EQ(n1.as_string(), std::string("Fido"));
    const auto& n2 = node.as_table().at("breed");
    ASSERT_EQ(n2.as_string(), std::string("pug"));

    const auto& owner = node.as_table().at("owner");
    ASSERT_EQ(owner.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(owner.as_table(), "name", "Regina Dogman"));
    const auto& dt = owner.as_table().at("member_since");
    ASSERT_TRUE(dt.is_datetime());
    ASSERT_EQ(dt.as_datetime().to_string(), "1999-08-04");
    const auto& detail = dt.as_datetime();
    ASSERT_EQ(detail.year, 1999);
    ASSERT_EQ(detail.month, 8);
    ASSERT_EQ(detail.day, 4);
}

TEST(Table, tab09) {
    std::string path = TEST_CASE_DIR "/tab09.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& fruit = node.as_table().at("fruit");
    ASSERT_EQ(fruit.as_table().size(), 1);
    const auto& y = fruit.as_table().at("apple");
    ASSERT_EQ(y.as_table().size(), 2);
    ASSERT_TRUE(test::CheckTableHasStringValue(y.as_table(), "color", "red"));
    const auto& taste = y.as_table().at("taste");
    ASSERT_EQ(taste.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasBoolValue(taste.as_table(), "sweet", true));
}

TEST(Table, tab10) {
    std::string path = TEST_CASE_DIR "/tab10.toml";
    auto result = toml::parse_file(path);
    ASSERT_FALSE(result.ok());
}

TEST(Table, tab11) {
    std::string path = TEST_CASE_DIR "/tab11.toml";
    auto result = toml::parse_file(path);
    ASSERT_TRUE(result.ok()) << result.error.to_string();
    const auto& node = result.value;

    const auto& fruit = node.as_table().at("fruit");
    ASSERT_EQ(fruit.as_table().size(), 1);
    const auto& y = fruit.as_table().at("apple");
    ASSERT_EQ(y.as_table().size(), 3);
    ASSERT_TRUE(test::CheckTableHasStringValue(y.as_table(), "color", "red"));
    const auto& taste = y.as_table().at("taste");
    ASSERT_EQ(taste.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasBoolValue(taste.as_table(), "sweet", true));

    const auto& texture = y.as_table().at("texture");
    ASSERT_EQ(texture.as_table().size(), 1);
    ASSERT_TRUE(test::CheckTableHasBoolValue(texture.as_table(), "smooth", true));
}


RUN_ALL_TESTS()
