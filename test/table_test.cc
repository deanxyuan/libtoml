#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckTableHasStringValue(TOML::Node node, const std::string &key, const std::string &value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == value;
}

bool CheckTableHasIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

bool CheckTableHasBoolValue(TOML::Node node, const std::string &key, bool value) {
    TOML::Node v = node.As<TOML::kTable>()->Get(key);
    if (v.Type() != TOML::kBoolean) {
        return false;
    }
    return v.As<TOML::kBoolean>()->Value() == value;
}

TEST(Table, tab01) {
    std::string path = TEST_CASE_DIR "/tab01.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node table = node.As<TOML::kTable>()->Get("table-1");
    ASSERT_EQ(table.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(table, "key1", "some string"));
    ASSERT_TRUE(CheckTableHasIntValue(table, "key2", 123));

    TOML::Node n2 = node.As<TOML::kTable>()->Get("table-2");
    ASSERT_EQ(n2.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(n2, "key1", "another string"));
    ASSERT_TRUE(CheckTableHasIntValue(n2, "key2", 456));
}

TEST(Table, tab02) {
    std::string path = TEST_CASE_DIR "/tab02.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    /*
        [dog."tater.man"]
        type.name = "pug"
    */
    TOML::Node dog = node.As<TOML::kTable>()->Get("dog");
    ASSERT_EQ(dog.As<TOML::kTable>()->size(), 1);
    TOML::Node man = dog.As<TOML::kTable>()->Get("tater.man");
    ASSERT_EQ(man.As<TOML::kTable>()->size(), 1);
    TOML::Node type = man.As<TOML::kTable>()->Get("type");
    ASSERT_EQ(type.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasStringValue(type, "name", "pug"));
}

TEST(Table, tab03) {
    std::string path = TEST_CASE_DIR "/tab03.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    /*
        [a.b.c]
    */
    TOML::Node a = node.As<TOML::kTable>()->Get("a");
    ASSERT_EQ(a.As<TOML::kTable>()->size(), 1);
    TOML::Node b = a.As<TOML::kTable>()->Get("b");
    ASSERT_EQ(b.As<TOML::kTable>()->size(), 1);
    TOML::Node c = b.As<TOML::kTable>()->Get("c");
    ASSERT_EQ(c.As<TOML::kTable>()->size(), 0);

    TOML::Node d = node.As<TOML::kTable>()->Get("d");
    ASSERT_EQ(d.As<TOML::kTable>()->size(), 1);
    TOML::Node e = d.As<TOML::kTable>()->Get("e");
    ASSERT_EQ(e.As<TOML::kTable>()->size(), 1);
    TOML::Node f = e.As<TOML::kTable>()->Get("f");
    ASSERT_EQ(f.As<TOML::kTable>()->size(), 0);

    TOML::Node g = node.As<TOML::kTable>()->Get("g");
    ASSERT_EQ(g.As<TOML::kTable>()->size(), 1);
    TOML::Node h = g.As<TOML::kTable>()->Get("h");
    ASSERT_EQ(h.As<TOML::kTable>()->size(), 1);
    TOML::Node i = h.As<TOML::kTable>()->Get("i");
    ASSERT_EQ(i.As<TOML::kTable>()->size(), 0);

    TOML::Node j = node.As<TOML::kTable>()->Get("j");
    ASSERT_EQ(j.As<TOML::kTable>()->size(), 1);
    TOML::Node k = j.As<TOML::kTable>()->Get("ʞ");
    ASSERT_EQ(k.As<TOML::kTable>()->size(), 1);
    TOML::Node l = k.As<TOML::kTable>()->Get("l");
    ASSERT_EQ(l.As<TOML::kTable>()->size(), 0);
}

TEST(Table, tab04) {
    std::string path = TEST_CASE_DIR "/tab04.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node x = node.As<TOML::kTable>()->Get("x");
    ASSERT_EQ(x.As<TOML::kTable>()->size(), 1);
    TOML::Node y = x.As<TOML::kTable>()->Get("y");
    ASSERT_EQ(y.As<TOML::kTable>()->size(), 1);
    TOML::Node z = y.As<TOML::kTable>()->Get("z");
    ASSERT_EQ(z.As<TOML::kTable>()->size(), 1);
    TOML::Node w = z.As<TOML::kTable>()->Get("w");
    ASSERT_EQ(w.As<TOML::kTable>()->size(), 0);
}

TEST(Table, tab05) {
    std::string path = TEST_CASE_DIR "/tab05.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Table, tab06) {
    std::string path = TEST_CASE_DIR "/tab06.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Table, tab07) {
    std::string path = TEST_CASE_DIR "/tab07.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kTable>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kTable>()->size(), 2);
    TOML::Node y = fruit.As<TOML::kTable>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kTable>()->size(), 0);
    TOML::Node z = fruit.As<TOML::kTable>()->Get("orange");
    ASSERT_EQ(z.As<TOML::kTable>()->size(), 0);
    TOML::Node w = node.As<TOML::kTable>()->Get("animal");
    ASSERT_EQ(w.As<TOML::kTable>()->size(), 0);
}

TEST(Table, tab08) {
    std::string path = TEST_CASE_DIR "/tab08.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kTable>()->Get("name");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("Fido"));
    TOML::Node n2 = node.As<TOML::kTable>()->Get("breed");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("pug"));

    TOML::Node owner = node.As<TOML::kTable>()->Get("owner");
    ASSERT_EQ(owner.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(owner, "name", "Regina Dogman"));
    TOML::Node dt = owner.As<TOML::kTable>()->Get("member_since");
    ASSERT_EQ(dt.Type(), TOML::kDateTime);
    ASSERT_EQ(dt.As<TOML::kDateTime>()->RawString(), "1999-08-04");
    auto detail = dt.As<TOML::kDateTime>()->Value();
    ASSERT_EQ(detail.Year(), 1999);
    ASSERT_EQ(detail.Month(), 8);
    ASSERT_EQ(detail.Day(), 4);
}

TEST(Table, tab09) {
    std::string path = TEST_CASE_DIR "/tab09.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kTable>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kTable>()->size(), 1);
    TOML::Node y = fruit.As<TOML::kTable>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kTable>()->size(), 2);
    ASSERT_TRUE(CheckTableHasStringValue(y, "color", "red"));
    TOML::Node taste = y.As<TOML::kTable>()->Get("taste");
    ASSERT_EQ(taste.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasBoolValue(taste, "sweet", true));
}
TEST(Table, tab10) {
    std::string path = TEST_CASE_DIR "/tab10.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}
TEST(Table, tab11) {
    std::string path = TEST_CASE_DIR "/tab11.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kTable>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kTable>()->size(), 1);
    TOML::Node y = fruit.As<TOML::kTable>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kTable>()->size(), 3);
    ASSERT_TRUE(CheckTableHasStringValue(y, "color", "red"));
    TOML::Node taste = y.As<TOML::kTable>()->Get("taste");
    ASSERT_EQ(taste.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasBoolValue(taste, "sweet", true));

    TOML::Node texture = y.As<TOML::kTable>()->Get("texture");
    ASSERT_EQ(texture.As<TOML::kTable>()->size(), 1);
    ASSERT_TRUE(CheckTableHasBoolValue(texture, "smooth", true));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
