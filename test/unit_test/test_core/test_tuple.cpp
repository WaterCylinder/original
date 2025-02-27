//
// Created by Lenovo on 25-2-27.
//
#include "tuple.h"
#include "array.h"
#include "chain.h"
#include <gtest/gtest.h>
#include <string>

using namespace original;

TEST(TupleTest, ConstructorAndGet) {
    tuple<int, double, std::string> t(42, 3.14, "hello");

    EXPECT_EQ(t.get<0>(), 42);
    EXPECT_DOUBLE_EQ(t.get<1>(), 3.14);
    EXPECT_EQ(t.get<2>(), "hello");
}

TEST(TupleTest, CopySemantics) {
    tuple<int, std::string> t1(10, "copy");
    tuple<int, std::string> t2(t1);

    EXPECT_EQ(t1.get<0>(), t2.get<0>());
    EXPECT_EQ(t1.get<1>(), t2.get<1>());

    t2.set<0>(20);
    EXPECT_NE(t1.get<0>(), t2.get<0>());
}

TEST(TupleTest, MoveSemantics) {
    const auto a = array{1, 3};
    const auto c = chain{2, 2, 4};
    const auto s = std::string{"1234"};
    auto t1 = tuple{a, c, s};
    auto t2 = tuple{std::move(t1)};
    EXPECT_EQ(t2.get<0>(), a);
    EXPECT_EQ(t2.get<1>(), c);
    EXPECT_EQ(t2.get<2>(), s);
    EXPECT_TRUE(t1.get<0>().empty());
    EXPECT_TRUE(t1.get<1>().empty());
    EXPECT_TRUE(t1.get<2>().empty());
    auto t3 = tuple{a, c, s};
    auto t4 = t3;
    std::cout << t4 << std::endl;
    t4 = std::move(t3);
    EXPECT_EQ(t4.get<0>(), a);
    EXPECT_EQ(t4.get<1>(), c);
    EXPECT_EQ(t4.get<2>(), s);
    EXPECT_TRUE(t3.get<0>().empty());
    EXPECT_TRUE(t3.get<1>().empty());
    EXPECT_TRUE(t3.get<2>().empty());
}

TEST(TupleTest, SizeMethod) {
    tuple<int> t1;
    tuple<int, int> t2;

    EXPECT_EQ(t1.size(), 1);
    EXPECT_EQ(t2.size(), 2);
}

TEST(TupleTest, SetMethod) {
    tuple<int, std::string> t(0, "");
    t.set<0>(100);
    t.set<1>("test");

    EXPECT_EQ(t.get<0>(), 100);
    EXPECT_EQ(t.get<1>(), "test");
}

TEST(TupleTest, SliceOperation) {
tuple<int, double, std::string, char> t(1, 2.3, "slice", 'A');

auto t1 = t.slice<1, 2>();
EXPECT_EQ(t1.size(), 2);
EXPECT_DOUBLE_EQ(t1.get<0>(), 2.3);
EXPECT_EQ(t1.get<1>(), "slice");
}

TEST(TupleTest, Concatenation) {
tuple<int, double> t1(1, 2.3);
tuple<std::string, char> t2("concat", 'X');

auto t3 = t1 + t2;
EXPECT_EQ(t3.size(), 4);
EXPECT_EQ(t3.get<2>(), "concat");
}

TEST(TupleTest, Comparison) {
tuple<int, std::string> t1(1, "apple");
tuple<int, std::string> t2(1, "banana");
tuple<int, std::string> t3(2, "apple");

EXPECT_TRUE(t1 < t2);
EXPECT_TRUE(t1 < t3);
EXPECT_EQ(t1.compareTo(t1), 0);
}

TEST(TupleTest, StringRepresentation) {
tuple<int, std::string> t(42, "test");
std::string str = t.toString(false);

EXPECT_TRUE(str.find("42") != std::string::npos);
EXPECT_TRUE(str.find("test") != std::string::npos);
EXPECT_EQ(t.className(), "tuple");
}

TEST(TupleTest, MakeTupleFromCouple) {
couple<int, std::string> cp(99, "couple");
auto t = makeTuple(cp);

EXPECT_EQ(t.get<0>(), 99);
EXPECT_EQ(t.get<1>(), "couple");
}
