#include <gtest/gtest.h>
#include "types.h"

using namespace original;

struct Point {
    int x, y;

    explicit Point(int x = 0, int y = 0) : x(x), y(y) {}

    static const Point origin;

    Point& operator=(const Point& other) {
        if (this == &other) return *this;

        x = other.x;
        y = other.y;
        return *this;
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !this->operator==(other);
    }

    bool operator<(const Point& other) const {
        return (x < other.x && y <= other.y) || (x <= other.x && y < other.y);
    }

    bool operator<=(const Point& other) const {
        return this->operator<(other) || this->operator==(other);
    }

    bool operator>(const Point& other) const {
        return !this->operator<=(other);
    }

    bool operator>=(const Point& other) const {
        return !this->operator<(other);
    }
};

const Point Point::origin = Point();

// 测试 CallbackOf 概念
template <typename Callback, typename ReturnType, typename... Args>
requires CallbackOf<Callback, ReturnType, Args...>
ReturnType test_callback(Callback callback, Args&&... args) {
    return callback(std::forward<Args>(args)...);
}

// 测试回调函数（返回类型为 bool，参数为 Point 和 int）
bool callback_example(const Point& p, int i) {
    return p.x + p.y == i;
}

// 测试没有返回值的回调函数
void callback_no_return(const Point&) {
    // 无返回值
}

// 测试返回类型不匹配的回调函数（会导致编译错误）
int callback_return_int(const Point& p) {
    return p.x + p.y;
}

TEST(ConceptTest, CallbackOfTest) {
    Point p1{1, 2};
    Point p2{3, 4};

    // 测试返回类型为 bool 的回调函数
    auto boolCallback = [](const Point& p, int i) -> bool {
        return p.x + p.y == i;
    };
    EXPECT_TRUE((test_callback<bool(const Point&, int), bool, const Point&, int>(boolCallback, p1, 3))); // p1.x + p1.y == 3
    EXPECT_FALSE((test_callback<bool(const Point&, int), bool, const Point&, int>(boolCallback, p2, 5))); // p2.x + p2.y != 5

    // 测试没有返回值的回调函数
    auto voidCallback = [](const Point&) {
        // 不做任何事情
    };
    EXPECT_NO_THROW((test_callback<void(const Point&), void, const Point&>(voidCallback, p1))); // 仅检查是否没有抛出异常

    // 测试返回类型不匹配的回调函数（返回类型为 int）
    auto intCallback = [](const Point& p) -> int {
        return p.x + p.y;
    };
    EXPECT_EQ((test_callback<int(const Point&), int, const Point&>(intCallback, p1)), 3); // p1.x + p1.y == 3

    // 测试返回类型不匹配时的编译失败：以下代码应编译失败
    // EXPECT_THROW((test_callback<int(const Point&), int, const Point&>(callback_example, p1, 3)), std::exception); // 编译时应失败

    // 测试回调函数返回类型不匹配的情况，确保编译时错误
    // 编译失败，假设 callback_return_int 应该返回 int 而不是 bool
    // EXPECT_THROW((test_callback<bool(const Point&, int), bool, const Point&, int>(callback_return_int, p1)), std::exception); // 编译时应失败
}

// 测试 Comparable 概念
template <typename T>
requires Comparable<T>
bool test_comparable(const T& a, const T& b) {
    return a == b;
}

// 测试 Compare 概念
template <typename Callback, typename TYPE>
requires Compare<Callback, TYPE>
bool test_compare(const TYPE& t1, const TYPE& t2, Callback& callback) {
    return callback(t1, t2);
}

// 测试 Condition 概念
template <typename Callback, typename TYPE>
requires Condition<Callback, TYPE>
bool test_condition(const TYPE& t, Callback& callback) {
    return callback(t);
}

// 测试 Operation 概念
template <typename Callback, typename TYPE>
requires Operation<Callback, TYPE>
void test_operation(TYPE& t, Callback& callback) {
    callback(t);
}

// 测试 SuperOf 概念
class Base {};
class Derived : public Base {};
class Unrelated {};

TEST(ConceptTest, ComparableTest) {
    Point p1{1, 2};
    Point p2{1, 2};
    Point p3{2, 3};
    Point p4{1, 2};
    Point p5{2, 2};
    Point p6{1, 3};

    // 测试 `Comparable` 概念
    EXPECT_TRUE(test_comparable(p1, p2)); // 相同值
    EXPECT_FALSE(test_comparable(p1, p3)); // 不同值
    EXPECT_FALSE(test_comparable(p1, Point::origin)); // 不同值
    EXPECT_TRUE(test_comparable(p1, p4)); // 相同值
    EXPECT_FALSE(test_comparable(p5, p6)); // 不同值
}

TEST(ConceptTest, CompareTest) {
    auto compareByX = [](const Point& a, const Point& b) -> bool {
        return a.x == b.x;
    };
    auto compareByY = [](const Point& a, const Point& b) -> bool {
        return a.y == b.y;
    };

    Point p1{1, 2};
    Point p2{1, 3};
    Point p3{2, 3};

    // 测试 `Compare` 概念，按 `x` 值比较
    EXPECT_TRUE(test_compare(p1, p2, compareByX));
    EXPECT_FALSE(test_compare(p2, p3, compareByX));

    // 测试 `Compare` 概念，按 `y` 值比较
    EXPECT_FALSE(test_compare(p1, p3, compareByY));
    EXPECT_TRUE(test_compare(p2, p3, compareByY));
}

TEST(ConceptTest, ConditionTest) {
    auto inFirstQuadrant = [](const Point& p) {
        return p.x > 0 && p.y > 0;
    };

    auto inPositiveX = [](const Point& p) {
        return p.x > 0;
    };

    Point p1{1, 2};
    Point p2{-1, 2};
    Point p3{0, 0};

    // 测试 `Condition` 概念：判断是否在第一象限
    EXPECT_TRUE(test_condition(p1, inFirstQuadrant));
    EXPECT_FALSE(test_condition(p2, inFirstQuadrant));
    EXPECT_FALSE(test_condition(p3, inFirstQuadrant));

    // 测试 `Condition` 概念：判断是否在正X轴
    EXPECT_TRUE(test_condition(p1, inPositiveX));
    EXPECT_FALSE(test_condition(p2, inPositiveX));
}

TEST(ConceptTest, OperationTest) {
    auto incrementX = [](Point& p) {
        p.x += 1;
    };

    auto decrementY = [](Point& p) {
        p.y -= 1;
    };

    Point p1{1, 2};
    Point p2{3, 4};

    test_operation(p1, incrementX);
    test_operation(p2, decrementY);

    // 测试 `Operation` 概念：增加 x 值
    EXPECT_EQ(p1.x, 2);
    EXPECT_EQ(p1.y, 2);  // y 不变
    EXPECT_EQ(p2.x, 3);  // x 不变
    EXPECT_EQ(p2.y, 3);  // y 减少
}

TEST(ConceptTest, SuperOfTest) {
    // 测试 `SuperOf` 概念
    EXPECT_TRUE((SuperOf<Base, Derived>));
    EXPECT_FALSE((SuperOf<Derived, Base>)); // Derived 不能是 Base 的子类
    EXPECT_FALSE((SuperOf<Base, Unrelated>)); // Unrelated 类
}

TEST(ConceptTest, ExtendsOfTest) {
    // 测试 `ExtendsOf` 概念
    EXPECT_TRUE((ExtendsOf<Base, Derived>));  // Derived 是 Base 的子类
    EXPECT_FALSE((ExtendsOf<Derived, Base>)); // Derived 不是 Base 的父类
    EXPECT_FALSE((ExtendsOf<Base, Unrelated>)); // Unrelated 类
}

TEST(IndexSequenceTest, MakeSequenceSizeCheck) {
    constexpr u_integer size1 = 3;
    constexpr u_integer size2 = 0;
    constexpr u_integer size3 = 5;

    constexpr auto seq0 = makeSequence<size1>();
    EXPECT_EQ(decltype(seq0)::size(), size1);

    constexpr auto seq1 = makeSequence<size2>();
    EXPECT_EQ(decltype(seq1)::size(), size2);

    constexpr auto seq2 = makeSequence<size3>();
    EXPECT_EQ(decltype(seq2)::size(), size3);
}