#include <gtest/gtest.h>
#include <unordered_set>
#include "zeit.h"

using namespace original;
using namespace original::literals;

// 简写类型
using duration = time::duration;
using unit = time::unit;

TEST(DurationTest, ConstructorAndValue) {
    duration d1(5, unit::SECOND);
    EXPECT_EQ(d1.value(unit::SECOND), 5);
    EXPECT_EQ(d1.value(unit::MILLISECOND), 5000);
    EXPECT_EQ(d1.value(unit::MICROSECOND), 5000000);
    EXPECT_EQ(d1.value(unit::NANOSECOND), 5000000000);
}

TEST(DurationTest, AdditionAndSubtraction) {
    duration d1(2, unit::SECOND);
    duration d2(3, unit::SECOND);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(unit::SECOND), 5);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(unit::SECOND), 1);

    auto neg = -d1;
    EXPECT_EQ(neg.value(unit::SECOND), -2);
}

TEST(DurationTest, MultiplicationAndDivision) {
    duration d(3, unit::SECOND);

    auto triple = d * 3;
    EXPECT_EQ(triple.value(unit::SECOND), 9);

    auto halve = triple / 3;
    EXPECT_EQ(halve.value(unit::SECOND), 3);

    duration d2(2, unit::SECOND);
    auto q = triple / d2;
    EXPECT_EQ(q.value(unit::NANOSECOND), (9LL * 1000000000LL) / (2LL * 1000000000LL));
    EXPECT_EQ(q.value(unit::NANOSECOND), 4); // 9 / 2 向下取整
}

TEST(DurationTest, DivFloating) {
    duration d1(10, unit::SECOND);
    duration d2(3, unit::SECOND);

    double result1 = d1.div(d2);
    EXPECT_DOUBLE_EQ(result1, 10.0 / 3.0);

    double result2 = d1.div(4, time::SECOND);
    EXPECT_DOUBLE_EQ(result2, 2.5);
}

TEST(DurationTest, BasicComparisons) {
    duration d1(1, time::SECOND);
    duration d2(1500, time::MILLISECOND);
    duration d3(2, time::SECOND);
    duration d4(1, time::SECOND); // same as d1
    duration d5(2000, time::MILLISECOND);

    // operator==
    EXPECT_TRUE(d1 == d4);
    EXPECT_FALSE(d1 == d2);
    EXPECT_TRUE(d3 == d5);

    // operator!=
    EXPECT_TRUE(d1 != d2);
    EXPECT_FALSE(d1 != d4);

    // operator<
    EXPECT_TRUE(d1 < d2);
    EXPECT_FALSE(d3 < d2);

    // operator>
    EXPECT_TRUE(d3 > d1);
    EXPECT_FALSE(d1 > d4);

    // operator<=
    EXPECT_TRUE(d1 <= d2);
    EXPECT_TRUE(d1 <= d4);
    EXPECT_FALSE(d3 <= d2);

    // operator>=
    EXPECT_TRUE(d3 >= d1);
    EXPECT_TRUE(d1 >= d4);
    EXPECT_FALSE(d1 >= d2);

    // <=> (C++20 三路比较)
    EXPECT_EQ((d1 <=> d4), std::strong_ordering::equal);
    EXPECT_EQ((d1 <=> d2), std::strong_ordering::less);
    EXPECT_EQ((d3 <=> d2), std::strong_ordering::greater);
}

TEST(DurationTest, HashEqualAndHashUse) {
    // todo: 有点问题，之后再修
    duration d1(2, time::SECOND);
    duration d2(2000, time::MILLISECOND); // equal in value
    duration d3(1, time::SECOND);         // not equal

    // equals()
    EXPECT_TRUE(d1.equals(d2));
    EXPECT_FALSE(d1.equals(d3));

    // toHash() == toHash() if equal
    EXPECT_EQ(d1.toHash(), d2.toHash());

    // toHash() != if not equal (大概率，不强要求)
    EXPECT_NE(d1.toHash(), d3.toHash());

    // 可在 unordered_set 中使用
    std::unordered_set<duration, hash<duration>> durSet;
    durSet.insert(d1);
    EXPECT_TRUE(durSet.contains(d2)); // hash 和 equals 一致
    EXPECT_FALSE(durSet.contains(d3));
}

TEST(DurationTest, StaticZero) {
    auto zero = duration::ZERO;
    EXPECT_EQ(zero.value(), 0);
    EXPECT_EQ(zero.value(unit::SECOND), 0);
}

TEST(DurationTest, AbsFunction) {
    duration d1(-10, unit::MILLISECOND);
    auto absVal = abs(d1);
    EXPECT_EQ(absVal.value(unit::MILLISECOND), 10);

    duration d2(20, unit::MILLISECOND);
    EXPECT_EQ(abs(d2).value(unit::MILLISECOND), 20);
}

TEST(DurationTest, IntegerLiterals) {
    EXPECT_EQ((1_ns).value(unit::NANOSECOND), 1);
    EXPECT_EQ((1_us).value(unit::NANOSECOND), 1000);
    EXPECT_EQ((1_ms).value(unit::NANOSECOND), 1000000);
    EXPECT_EQ((1_s).value(unit::NANOSECOND), 1000000000);
    EXPECT_EQ((1_min).value(unit::NANOSECOND), 60LL * 1000000000);
    EXPECT_EQ((1_h).value(unit::NANOSECOND), 60LL * 60 * 1000000000);
}

TEST(DurationTest, IntegerLiteralConversionToMilliseconds) {
    EXPECT_EQ((1500_ms).value(unit::SECOND), 1);  // 1500ms = 1.5s, truncated to 1
    EXPECT_EQ((120_s).value(unit::MINUTE), 2);    // 120s = 2min
}

TEST(DurationTest, FloatingPointLiteralsRoundedDown) {
    EXPECT_EQ((1.5_s).value(unit::MILLISECOND), 1500);     // 1.5秒 = 1500ms
    EXPECT_EQ((2.25_min).value(unit::SECOND), 135);        // 2.25分钟 = 135秒
    EXPECT_EQ((0.001_ms).value(unit::NANOSECOND), 1000);   // 1微秒
    EXPECT_EQ((0.000001_s).value(unit::MICROSECOND), 1);   // 1微秒
}

TEST(DurationTest, FloatingPointPrecisionLimits) {
    auto d1 = 1.234567_s;
    EXPECT_NEAR(d1.value(unit::MILLISECOND), 1234, 1); // 1.234567秒 ≈ 1234.567ms，整数截断

    auto d2 = 0.000000001_h;
    EXPECT_EQ(d2.value(unit::NANOSECOND), 3600); // 1ns = 1e-9小时 → 3600ns
}