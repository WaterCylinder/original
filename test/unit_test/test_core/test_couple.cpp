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
    EXPECT_EQ(c.toString(true), "couple(10, 20.5)\n");
    EXPECT_EQ(c.toString(false), "couple(10, 20.5)");
}

// 测试相等运算符
TEST(CoupleTest, EqualityOperator) {
    couple<int, double> c1(10, 20.5);
    couple<int, double> c2(10, 20.5);
    couple<int, double> c3(5, 15.5);

    EXPECT_TRUE(c1 == c2); // 相等
    EXPECT_FALSE(c1 == c3); // 不相等
}

TEST(CoupleTest, GetMethodVariants) {
    couple<std::string, int> c("abc", 42);

    // 非常量对象调用 -> 匹配 auto&
    std::string& s1 = c.template get<0>();
    int& i1 = c.template get<1>();
    s1 = "def";
    i1 = 99;
    EXPECT_EQ(c.first(), "def");
    EXPECT_EQ(c.second(), 99);

    // 常量对象调用 -> 匹配 const auto&
    const couple<std::string, int>& cc = c;
    const std::string& s2 = cc.template get<0>();
    const int& i2 = cc.template get<1>();
    EXPECT_EQ(s2, "def");
    EXPECT_EQ(i2, 99);

    // 右值对象调用 -> 匹配 auto&&
    couple<std::string, int> temp("move", 123);
    std::string&& moved_str = std::move(temp.template get<0>());
    EXPECT_EQ(moved_str, "move");
}

TEST(CoupleTest, StructuredBinding) {
    couple<std::string, int> c("hello", 2025);
    auto&& [text, year] = c;  // 此处 auto&& 可确保兼容 get<>() 的重载
    EXPECT_EQ(text, "hello");
    EXPECT_EQ(year, 2025);

    text = "modified";
    EXPECT_EQ(c.first(), "modified");  // 结构化绑定绑定的是引用
}

TEST(CoupleTest, SetMethod) {
    couple<std::string, int> c("initial", 0);

    // Test setting first element
    c.set<0>("new value");
    EXPECT_EQ(c.first(), "new value");
    EXPECT_EQ(c.second(), 0);

    // Test setting second element
    c.set<1>(42);
    EXPECT_EQ(c.first(), "new value");
    EXPECT_EQ(c.second(), 42);

    // Test setting with implicit conversion
    c.set<1>(3.14);  // double to int conversion
    EXPECT_EQ(c.second(), 3);  // should truncate to int
}

TEST(CoupleTest, SetMethodEdgeCases) {
    couple<int, double> c(0, 0.0);

    // Test setting with different but convertible types
    float f = 1.5f;
    c.set<0>(f);  // float to int
    EXPECT_EQ(c.first(), 1);

    short s = 2;
    c.set<1>(s);  // short to double
    EXPECT_DOUBLE_EQ(c.second(), 2.0);

    // Test chain setting
    c.set<0>(10).set<1>(20.5);
    EXPECT_EQ(c.first(), 10);
    EXPECT_DOUBLE_EQ(c.second(), 20.5);
}

TEST(CoupleTest, ToStringWithSet) {
    couple<std::string, int> c("hello", 0);
    c.set<1>(42);
    EXPECT_EQ(c.toString(false), "couple(\"hello\", 42)");
}
