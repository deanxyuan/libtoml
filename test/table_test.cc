#include "toml/toml.h"
#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing Macro Definition: TEST_CASE_DIR, please check the CMakeLists.txt"
#endif

bool CheckObjectStringValue(TOML::Node node, const std::string &key, const std::string &value) {
    TOML::Node v = node.As<TOML::kObject>()->Get(key);
    if (v.Type() != TOML::kString) {
        return false;
    }
    return v.As<TOML::kString>()->Value() == value;
}

bool CheckObjectIntValue(TOML::Node node, const std::string &key, int64_t value) {
    TOML::Node v = node.As<TOML::kObject>()->Get(key);
    if (v.Type() != TOML::kInteger) {
        return false;
    }
    return v.As<TOML::kInteger>()->Value() == value;
}

bool CheckObjectBoolValue(TOML::Node node, const std::string &key, bool value) {
    TOML::Node v = node.As<TOML::kObject>()->Get(key);
    if (v.Type() != TOML::kBoolean) {
        return false;
    }
    return v.As<TOML::kBoolean>()->Value() == value;
}

TEST(Table, Basic) {
    std::string path = TEST_CASE_DIR "/tab01.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);
    TOML::Node table = node.As<TOML::kObject>()->Get("table-1");
    ASSERT_EQ(table.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(table, "key1", "some string"));
    ASSERT_TRUE(CheckObjectIntValue(table, "key2", 123));

    TOML::Node n2 = node.As<TOML::kObject>()->Get("table-2");
    ASSERT_EQ(n2.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(n2, "key1", "another string"));
    ASSERT_TRUE(CheckObjectIntValue(n2, "key2", 456));
}

TEST(Table, ComplexTitle) {
    std::string path = TEST_CASE_DIR "/tab02.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    /*
        [dog."tater.man"]
        type.name = "pug"
    */
    TOML::Node dog = node.As<TOML::kObject>()->Get("dog");
    ASSERT_EQ(dog.As<TOML::kObject>()->size(), 1);
    TOML::Node man = dog.As<TOML::kObject>()->Get("tater.man");
    ASSERT_EQ(man.As<TOML::kObject>()->size(), 1);
    TOML::Node type = man.As<TOML::kObject>()->Get("type");
    ASSERT_EQ(type.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectStringValue(type, "name", "pug"));
}

TEST(Table, ComplexTitleEmptyTable) {
    std::string path = TEST_CASE_DIR "/tab03.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    /*
        [a.b.c]
    */
    TOML::Node a = node.As<TOML::kObject>()->Get("a");
    ASSERT_EQ(a.As<TOML::kObject>()->size(), 1);
    TOML::Node b = a.As<TOML::kObject>()->Get("b");
    ASSERT_EQ(b.As<TOML::kObject>()->size(), 1);
    TOML::Node c = b.As<TOML::kObject>()->Get("c");
    ASSERT_EQ(c.As<TOML::kObject>()->size(), 0);

    TOML::Node d = node.As<TOML::kObject>()->Get("d");
    ASSERT_EQ(d.As<TOML::kObject>()->size(), 1);
    TOML::Node e = d.As<TOML::kObject>()->Get("e");
    ASSERT_EQ(e.As<TOML::kObject>()->size(), 1);
    TOML::Node f = e.As<TOML::kObject>()->Get("f");
    ASSERT_EQ(f.As<TOML::kObject>()->size(), 0);

    TOML::Node g = node.As<TOML::kObject>()->Get("g");
    ASSERT_EQ(g.As<TOML::kObject>()->size(), 1);
    TOML::Node h = g.As<TOML::kObject>()->Get("h");
    ASSERT_EQ(h.As<TOML::kObject>()->size(), 1);
    TOML::Node i = h.As<TOML::kObject>()->Get("i");
    ASSERT_EQ(i.As<TOML::kObject>()->size(), 0);

    TOML::Node j = node.As<TOML::kObject>()->Get("j");
    ASSERT_EQ(j.As<TOML::kObject>()->size(), 1);
    TOML::Node k = j.As<TOML::kObject>()->Get("ʞ");
    ASSERT_EQ(k.As<TOML::kObject>()->size(), 1);
    TOML::Node l = k.As<TOML::kObject>()->Get("l");
    ASSERT_EQ(l.As<TOML::kObject>()->size(), 0);
}

TEST(Table, ComplexTitleAfterwardParent) {
    std::string path = TEST_CASE_DIR "/tab04.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node x = node.As<TOML::kObject>()->Get("x");
    ASSERT_EQ(x.As<TOML::kObject>()->size(), 1);
    TOML::Node y = x.As<TOML::kObject>()->Get("y");
    ASSERT_EQ(y.As<TOML::kObject>()->size(), 1);
    TOML::Node z = y.As<TOML::kObject>()->Get("z");
    ASSERT_EQ(z.As<TOML::kObject>()->size(), 1);
    TOML::Node w = z.As<TOML::kObject>()->Get("w");
    ASSERT_EQ(w.As<TOML::kObject>()->size(), 0);
}

TEST(Table, RedefineTest1) {
    std::string path = TEST_CASE_DIR "/tab05.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}
TEST(Table, RedefineTest2) {
    std::string path = TEST_CASE_DIR "/tab06.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}

TEST(Table, ComplexTitleMultiPointAdd) {
    std::string path = TEST_CASE_DIR "/tab07.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kObject>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kObject>()->size(), 2);
    TOML::Node y = fruit.As<TOML::kObject>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kObject>()->size(), 0);
    TOML::Node z = fruit.As<TOML::kObject>()->Get("orange");
    ASSERT_EQ(z.As<TOML::kObject>()->size(), 0);
    TOML::Node w = node.As<TOML::kObject>()->Get("animal");
    ASSERT_EQ(w.As<TOML::kObject>()->size(), 0);
}

TEST(Table, TopLevelTable) {
    std::string path = TEST_CASE_DIR "/tab08.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node n1 = node.As<TOML::kObject>()->Get("name");
    ASSERT_EQ(n1.As<TOML::kString>()->Value(), std::string("Fido"));
    TOML::Node n2 = node.As<TOML::kObject>()->Get("breed");
    ASSERT_EQ(n2.As<TOML::kString>()->Value(), std::string("pug"));

    TOML::Node owner = node.As<TOML::kObject>()->Get("owner");
    ASSERT_EQ(owner.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(owner, "name", "Regina Dogman"));
    TOML::Node dt = owner.As<TOML::kObject>()->Get("member_since");
    ASSERT_EQ(dt.Type(), TOML::kDateTime);
    ASSERT_EQ(dt.As<TOML::kDateTime>()->RawString(), "1999-08-04");
    auto detail = dt.As<TOML::kDateTime>()->Value();
    ASSERT_EQ(detail.Year(), 1999);
    ASSERT_EQ(detail.Month(), 8);
    ASSERT_EQ(detail.Day(), 4);
}

TEST(Table, ComplexTitleMultiPointAdd2) {
    std::string path = TEST_CASE_DIR "/tab09.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kObject>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kObject>()->size(), 1);
    TOML::Node y = fruit.As<TOML::kObject>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kObject>()->size(), 2);
    ASSERT_TRUE(CheckObjectStringValue(y, "color", "red"));
    TOML::Node taste = y.As<TOML::kObject>()->Get("taste");
    ASSERT_EQ(taste.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectBoolValue(taste, "sweet", true));
}
TEST(Table, RedefineTest3) {
    std::string path = TEST_CASE_DIR "/tab10.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_FALSE(error.empty());
    ASSERT_FALSE(node);
}
TEST(Table, ComplexTitleMultiPointAdd3) {
    std::string path = TEST_CASE_DIR "/tab11.toml";
    std::string error;
    TOML::Node node = TOML::LoadFromFile(path, &error);
    ASSERT_TRUE(error.empty());
    ASSERT_TRUE(node);

    TOML::Node fruit = node.As<TOML::kObject>()->Get("fruit");
    ASSERT_EQ(fruit.As<TOML::kObject>()->size(), 1);
    TOML::Node y = fruit.As<TOML::kObject>()->Get("apple");
    ASSERT_EQ(y.As<TOML::kObject>()->size(), 3);
    ASSERT_TRUE(CheckObjectStringValue(y, "color", "red"));
    TOML::Node taste = y.As<TOML::kObject>()->Get("taste");
    ASSERT_EQ(taste.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectBoolValue(taste, "sweet", true));

    TOML::Node texture = y.As<TOML::kObject>()->Get("texture");
    ASSERT_EQ(texture.As<TOML::kObject>()->size(), 1);
    ASSERT_TRUE(CheckObjectBoolValue(texture, "smooth", true));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
