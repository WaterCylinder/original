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

// 简单的move-only类型
class MoveOnly {
    std::unique_ptr<int> data_;

public:
    explicit MoveOnly(int value = 0) : data_(std::make_unique<int>(value)) {}

    // 删除拷贝构造函数和拷贝赋值
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    // 允许移动构造函数和移动赋值
    MoveOnly(MoveOnly&& other) noexcept = default;
    MoveOnly& operator=(MoveOnly&& other) noexcept = default;

    [[nodiscard]] int value() const { return data_ ? *data_ : -1; }

    bool operator==(const MoveOnly& other) const { return *data_ == *other.data_; }
    bool operator!=(const MoveOnly& other) const { return *data_ != *other.data_; }
    bool operator<(const MoveOnly& other) const { return *data_ < *other.data_; }
    bool operator>(const MoveOnly& other) const { return *data_ > *other.data_; }
    bool operator<=(const MoveOnly& other) const { return *data_ <= *other.data_; }
    bool operator>=(const MoveOnly& other) const { return *data_ >= *other.data_; }

    friend std::ostream& operator<<(std::ostream& os, const MoveOnly& mo) {
        return os << mo.value();
    }
};

// 测试move-only类型的默认构造
TEST(CoupleMoveOnlyTest, DefaultConstructor) {
    couple<MoveOnly, MoveOnly> c;
    EXPECT_EQ(c.first().value(), 0);
    EXPECT_EQ(c.second().value(), 0);
}

// 测试move-only类型的移动构造
TEST(CoupleMoveOnlyTest, MoveConstructor) {
    MoveOnly first(42);
    MoveOnly second(100);

    couple c1(std::move(first), std::move(second));
    EXPECT_EQ(c1.first().value(), 42);
    EXPECT_EQ(c1.second().value(), 100);

    // 移动后原对象应该处于有效但未定义的状态
    // (对于unique_ptr，移动后变为nullptr)
}

// 测试couple的移动构造
TEST(CoupleMoveOnlyTest, CoupleMoveConstructor) {
    couple c1(MoveOnly(10), MoveOnly(20));
    couple c2(std::move(c1));

    EXPECT_EQ(c2.first().value(), 10);
    EXPECT_EQ(c2.second().value(), 20);
}

// 测试couple的移动赋值
TEST(CoupleMoveOnlyTest, CoupleMoveAssignment) {
    couple c1(MoveOnly(30), MoveOnly(40));

    couple<MoveOnly, MoveOnly> c2 = std::move(c1);

    EXPECT_EQ(c2.first().value(), 30);
    EXPECT_EQ(c2.second().value(), 40);
}

// 测试get方法对move-only类型的支持
TEST(CoupleMoveOnlyTest, GetMethod) {
    couple c(MoveOnly(50), MoveOnly(60));

    // 获取非常量引用
    MoveOnly& first = c.get<0>();
    MoveOnly& second = c.get<1>();

    EXPECT_EQ(first.value(), 50);
    EXPECT_EQ(second.value(), 60);

    // 修改值
    first = MoveOnly(70);
    second = MoveOnly(80);

    EXPECT_EQ(c.first().value(), 70);
    EXPECT_EQ(c.second().value(), 80);
}

// 测试结构化绑定与move-only类型
TEST(CoupleMoveOnlyTest, StructuredBinding) {
    couple<MoveOnly, MoveOnly> c(MoveOnly(90), MoveOnly(100));

    auto&& [first, second] = c;
    EXPECT_EQ(first.value(), 90);
    EXPECT_EQ(second.value(), 100);

    // 修改值
    first = MoveOnly(110);
    second = MoveOnly(120);

    EXPECT_EQ(c.first().value(), 110);
    EXPECT_EQ(c.second().value(), 120);
}

// 测试move-only类型的比较操作
TEST(CoupleMoveOnlyTest, Comparison) {
    couple c1(MoveOnly(1), MoveOnly(2));
    couple c2(MoveOnly(1), MoveOnly(2));
    couple c3(MoveOnly(3), MoveOnly(4));

    EXPECT_EQ(c1.compareTo(c2), 0);  // 相等
    EXPECT_LT(c1.compareTo(c3), 0);  // c1 < c3
}

// 测试包含std::unique_ptr的couple
TEST(CoupleMoveOnlyTest, WithUniquePtr) {
    couple<std::unique_ptr<int>, std::unique_ptr<std::string>> c;

    c.first() = std::make_unique<int>(42);
    c.second() = std::make_unique<std::string>("hello");

    EXPECT_EQ(*c.first(), 42);
    EXPECT_EQ(*c.second(), "hello");

    // 测试移动
    auto c2 = std::move(c);
    EXPECT_EQ(*c2.first(), 42);
    EXPECT_EQ(*c2.second(), "hello");

    // 原对象的指针应该为nullptr
    EXPECT_EQ(c.first(), nullptr); // NOLINT: Move test
    EXPECT_EQ(c.second(), nullptr);
}

// 测试toString对move-only类型的支持
TEST(CoupleMoveOnlyTest, ToString) {
    const couple c(MoveOnly(123), MoveOnly(456));

    const std::string result = c.toString(false);
    EXPECT_TRUE(result.find("123") != std::string::npos);
    EXPECT_TRUE(result.find("456") != std::string::npos);
}

// 测试混合类型：move-only + copyable
TEST(CoupleMoveOnlyTest, MixedTypes) {
    couple<std::unique_ptr<int>, std::string> c;

    c.first() = std::make_unique<int>(999);
    c.second() = "test string";

    EXPECT_EQ(*c.first(), 999);
    EXPECT_EQ(c.second(), "test string");

    // 测试移动构造
    auto c2 = std::move(c);
    EXPECT_EQ(*c2.first(), 999);
    EXPECT_EQ(c2.second(), "test string");
}
