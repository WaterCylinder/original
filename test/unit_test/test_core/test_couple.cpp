#include "gtest/gtest.h"
#include "couple.h"

using namespace original;

// 测试构造函数
TEST(CoupleTest, DefaultConstructor) {
    couple<int, double> c;
    EXPECT_EQ(c.first(), 0);
    EXPECT_EQ(c.second(), 0.0);
}

TEST(CoupleTest, ParameterizedConstructor) {
    int f = 10;
    double s = 20.5;
    couple<int, double> c(&f, &s);
    EXPECT_EQ(c.first(), 10);
    EXPECT_EQ(c.second(), 20.5);
}

TEST(CoupleTest, CopyConstructor) {
    couple<int, double> c1(10, 20.5);
    const couple<int, double>& c2 = c1; // 使用拷贝构造函数
    EXPECT_EQ(c1, c2); // 通过重载的==运算符检查是否相等
}

TEST(CoupleTest, AssignmentOperator) {
    couple<int, double> c1(10, 20.5);
    couple<int, double> c2(5, 15.5);
    c2 = c1; // 使用赋值操作符
    EXPECT_EQ(c1, c2); // 通过重载的==运算符检查是否相等
}

// 测试成员函数
TEST(CoupleTest, FirstAndSecond) {
    couple<int, double> c(10, 20.5);
    EXPECT_EQ(c.first(), 10);
    EXPECT_EQ(c.second(), 20.5);
}

TEST(CoupleTest, ToString) {
    couple<int, double> c(10, 20.5);
    EXPECT_EQ(c.toString(true), "couple(first: 10, second: 20.5)\n");
    EXPECT_EQ(c.toString(false), "couple(first: 10, second: 20.5)");
}

// 测试相等运算符
TEST(CoupleTest, EqualityOperator) {
    couple<int, double> c1(10, 20.5);
    couple<int, double> c2(10, 20.5);
    couple<int, double> c3(5, 15.5);

    EXPECT_TRUE(c1 == c2); // 相等
    EXPECT_FALSE(c1 == c3); // 不相等
}
