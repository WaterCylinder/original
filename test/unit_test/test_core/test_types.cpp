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

// 测试 Comparable 概念
template <typename T>
requires Comparable<T>
bool test_comparable(const T& a, const T& b) {
    return a == b;
}

// 测试 Compare 概念
template <typename Callback, typename TYPE>
requires Comparable<TYPE> && Compare<Callback, TYPE>
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

TEST(ConceptTest, ComparableTest) {
Point p1{1, 2};
Point p2{1, 2};
Point p3{2, 3};

// 测试 `Comparable` 概念
EXPECT_TRUE(test_comparable(p1, p2));
EXPECT_FALSE(test_comparable(p1, p3));
EXPECT_FALSE(test_comparable(p1, Point::origin));
}

TEST(ConceptTest, CompareTest) {
auto compare = [](const Point& a, const Point& b) -> bool {
    return a == b;
};

Point p1{1, 2};
Point p2{1, 2};
Point p3{2, 3};

// 测试 `Compare` 概念
EXPECT_TRUE(test_compare(p1, p2, compare));
EXPECT_FALSE(test_compare(p1, p3, compare));
EXPECT_FALSE(test_compare(p2, Point::origin, compare));
}

TEST(ConceptTest, ConditionTest) {
auto inFirstQuadrant = [](const Point& p) {
    return p.x > 0 && p.y > 0;
};

Point p1{1, 2};
Point p2{-1, 2};

// 测试 `Condition` 概念
EXPECT_TRUE(test_condition(p1, inFirstQuadrant));
EXPECT_FALSE(test_condition(p2, inFirstQuadrant));
EXPECT_FALSE(test_condition(Point::origin, inFirstQuadrant));
}

TEST(ConceptTest, OperationTest) {
auto operation = [](Point& p) {
    p.x += 1;
    p.y += 1;
};

Point p1{1, 2};
Point p2 = Point::origin;
Point p3{1, 1};
test_operation(p1, operation);
test_operation(p2, operation);
// 测试 `Operation` 概念
EXPECT_EQ(p1.x, 2);
EXPECT_EQ(p1.y, 3);
EXPECT_EQ(p2, p3);
}

TEST(ConceptTest, SuperOfTest) {
// 测试 `SuperOf` 概念
EXPECT_TRUE((SuperOf<Base, Derived>));
EXPECT_FALSE((SuperOf<Derived, Base>));
}

TEST(ConceptTest, ExtendsOfTest) {
// 测试 `ExtendsOf` 概念
EXPECT_TRUE((ExtendsOf<Base, Derived>));
EXPECT_FALSE((ExtendsOf<Derived, Base>));
}

