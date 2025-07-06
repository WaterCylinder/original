#include <gtest/gtest.h>
#include <unordered_set>
#include "zeit.h"

using namespace original;
using namespace original::literals;

// 简写类型
using duration = time::duration;
using point = time::point;
using unit = time::unit;

TEST(DurationTest, ConstructorAndValue) {
    const duration d1(5, unit::SECOND);
    EXPECT_EQ(d1.value(unit::SECOND), 5);
    EXPECT_EQ(d1.value(unit::MILLISECOND), 5000);
    EXPECT_EQ(d1.value(unit::MICROSECOND), 5000000);
    EXPECT_EQ(d1.value(unit::NANOSECOND), 5000000000);
    EXPECT_EQ(d1.value(unit::DAY), 0); // 5秒 < 1天，取整应为0

    duration d2(2, unit::DAY);
    EXPECT_EQ(d2.value(unit::SECOND), 2 * 24 * 60 * 60);
    EXPECT_EQ(d2.value(unit::HOUR), 2 * 24);
    EXPECT_EQ(d2.value(unit::MINUTE), 2 * 24 * 60);
}

TEST(DurationTest, AdditionAndSubtraction) {
    const duration d1(2, unit::SECOND);
    const duration d2(3, unit::SECOND);

    const auto sum = d1 + d2;
    EXPECT_EQ(sum.value(unit::SECOND), 5);

    const auto diff = d2 - d1;
    EXPECT_EQ(diff.value(unit::SECOND), 1);

    const auto neg = -d1;
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
    duration one_day(1, unit::DAY);
    duration two_days = one_day * 2;
    EXPECT_EQ(two_days.value(unit::DAY), 2);

    duration half_day = two_days / 4;
    EXPECT_EQ(half_day.value(unit::HOUR), 12);
}

TEST(DurationTest, DivFloating) {
    const duration d1(10, unit::SECOND);
    const duration d2(3, unit::SECOND);

    const double result1 = d1.div(d2);
    EXPECT_DOUBLE_EQ(result1, 10.0 / 3.0);

    const double result2 = d1.div(4, time::SECOND);
    EXPECT_DOUBLE_EQ(result2, 2.5);
    const duration d_day(1, unit::DAY);
    const double hours = d_day.div(1, unit::HOUR);
    EXPECT_DOUBLE_EQ(hours, 24.0);

    const duration two_days(2, unit::DAY);
    const double ratio = two_days.div(d_day);
    EXPECT_DOUBLE_EQ(ratio, 2.0);

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
    const duration d1(2, time::SECOND);
    const duration d2(2000, time::MILLISECOND); // equal in value
    const duration d3(1, time::SECOND);         // not equal

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
    const auto zero = duration::ZERO;
    EXPECT_EQ(zero.value(), 0);
    EXPECT_EQ(zero.value(unit::SECOND), 0);
}

TEST(DurationTest, AbsFunction) {
    const duration d1(-10, unit::MILLISECOND);
    const auto absVal = abs(d1);
    EXPECT_EQ(absVal.value(unit::MILLISECOND), 10);

    const duration d2(20, unit::MILLISECOND);
    EXPECT_EQ(abs(d2).value(unit::MILLISECOND), 20);
}

TEST(DurationTest, IntegerLiterals) {
    EXPECT_EQ((1_ns).value(unit::NANOSECOND), 1);
    EXPECT_EQ((1_us).value(unit::NANOSECOND), 1000);
    EXPECT_EQ((1_ms).value(unit::NANOSECOND), 1000000);
    EXPECT_EQ((1_s).value(unit::NANOSECOND), 1000000000);
    EXPECT_EQ((1_min).value(unit::NANOSECOND), 60LL * 1000000000);
    EXPECT_EQ((1_h).value(unit::NANOSECOND), 60LL * 60 * 1000000000);
    EXPECT_EQ((1_d).value(unit::HOUR), 24);
    EXPECT_EQ((1_d).value(unit::SECOND), 86400);
    EXPECT_EQ((2_d).value(unit::DAY), 2);
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
    const auto d1 = 1.234567_s;
    EXPECT_NEAR(d1.value(unit::MILLISECOND), 1234, 1); // 1.234567秒 ≈ 1234.567ms，整数截断

    const auto d2 = 0.000000001_h;
    EXPECT_EQ(d2.value(unit::NANOSECOND), 3600); // 1ns = 1e-9小时 → 3600ns
}

TEST(TimePointTest, BasicConstructionAndValue) {
    const point p1(1, time::SECOND);
    EXPECT_EQ(p1.value(time::SECOND), 1);
    EXPECT_EQ(p1.value(time::MILLISECOND), 1000);
    EXPECT_EQ(p1.value(time::NANOSECOND), 1'000'000'000);
    const point p1d(1, time::DAY);
    EXPECT_EQ(p1d.value(time::HOUR), 24);
    EXPECT_EQ(p1d.value(time::SECOND), 86400);
}

TEST(TimePointTest, ConstructionFromDuration) {
    const duration d(500, time::MILLISECOND);
    const point p(d);
    EXPECT_EQ(p.value(time::MILLISECOND), 500);
}

TEST(TimePointTest, NowFunctionShouldBeGreaterThanZero) {
    const auto now = point::now();
    EXPECT_GT(now.value(time::SECOND), 0);  // 假设机器时间正确
}

TEST(TimePointTest, PointAdditionAndSubtraction) {
    point p1(1, time::SECOND);
    duration d(500, time::MILLISECOND);
    point p2 = p1 + d;
    EXPECT_EQ(p2.value(time::MILLISECOND), 1500);

    point p3 = p2 - d;
    EXPECT_EQ(p3.value(time::MILLISECOND), 1000);

    point p_day(0, time::SECOND);
    duration one_day(1, time::DAY);
    point p_next = p_day + one_day;

    EXPECT_EQ(p_next.value(time::DAY), 1);
    EXPECT_EQ((p_next - p_day).value(time::HOUR), 24);
}

TEST(TimePointTest, PointMinusPointReturnsDuration) {
    const point p1(2, time::SECOND);
    const point p2(500, time::MILLISECOND);
    const duration d = p1 - p2;
    EXPECT_EQ(d.value(time::MILLISECOND), 1500);
}

TEST(TimePointTest, PointCompare) {
    const point p1(1, time::SECOND);
    const point p2(2, time::SECOND);

    EXPECT_LT(p1, p2);
    EXPECT_GT(p2, p1);
    EXPECT_EQ(p1, p1);
}

TEST(TimePointTest, HashAndEquality) {
    const point p1(123456, time::MICROSECOND);
    const point p2(123456, time::MICROSECOND);
    const point p3(123457, time::MICROSECOND);

    EXPECT_EQ(p1, p2);
    EXPECT_EQ(p1.toHash(), p2.toHash());
    EXPECT_NE(p1, p3);
}

TEST(UTCTimeTest, Construction) {
    time::UTCTime t1; // 默认构造应为 1970-01-01 00:00:00
    EXPECT_EQ(t1.toString(false), "(time::UTCTime 1970-01-01 00:00:00)");
    t1 = time::UTCTime::EPOCH;
    EXPECT_EQ(t1.toString(false), "(time::UTCTime 1970-01-01 00:00:00)");

    time::UTCTime t2(2024, 6, 1, 12, 30, 45);
    EXPECT_EQ(t2.value(time::YEAR), 2024);
    EXPECT_EQ(t2.value(time::MONTH), 6);
    EXPECT_EQ(t2.value(time::DAY), 1);
    EXPECT_EQ(t2.value(time::HOUR), 12);
    EXPECT_EQ(t2.value(time::MINUTE), 30);
    EXPECT_EQ(t2.value(time::SECOND), 45);

    time::point p {time::UTCTime(2024, 6, 1, 12, 0, 0)}; // 转换为point再还原
    time::UTCTime t3(p);
    EXPECT_EQ(t3.value(time::YEAR), 2024);
    EXPECT_EQ(t3.value(time::MONTH), 6);
    EXPECT_EQ(t3.value(time::DAY), 1);
    EXPECT_EQ(t3.value(time::HOUR), 12);
}

TEST(UTCTimeTest, DurationAddSubtract) {
    const time::UTCTime t(2024, 1, 1, 0, 0, 0);
    const auto t_plus = t + 1_d;
    EXPECT_EQ(t_plus.value(time::DAY), 2);
    EXPECT_EQ(t_plus, time::UTCTime (2024, 1, 2, 0, 0, 0));

    const auto t_minus = t_plus - 1_d;
    EXPECT_EQ(t_minus.toString(false), t.toString(false));
    EXPECT_EQ(t_minus, t);

    const auto d = t_plus - t;
    EXPECT_EQ(d.value(time::HOUR), 24);
}

TEST(UTCTimeTest, ComparisonOperators) {
    time::UTCTime t1(2024, 6, 1, 0, 0, 0);
    time::UTCTime t2(2024, 6, 1, 0, 0, 1);
    time::UTCTime t3(2024, 5, 31, 23, 59, 59);
    EXPECT_TRUE(t1 < t2);
    EXPECT_TRUE(t1 <= t2);
    EXPECT_TRUE(t2 > t1);
    EXPECT_TRUE(t2 >= t1);
    EXPECT_FALSE(t1 == t2);
    EXPECT_TRUE(t1 != t2);

    EXPECT_TRUE(t3 < t2);
    EXPECT_TRUE(t3 <= t2);
    EXPECT_FALSE(t3 > t1);
    EXPECT_FALSE(t3 >= t1);
    EXPECT_FALSE(t3 == t2);
    EXPECT_TRUE(t3 != t2);
}

TEST(UTCTimeTest, HashFunctionality) {
    const time::UTCTime t1(2023, 12, 25, 10, 30, 0);
    const time::UTCTime t2(2023, 12, 25, 10, 30, 0);
    const time::UTCTime t3(2024, 1, 1, 0, 0, 0);

    EXPECT_TRUE(t1 == t2);
    EXPECT_EQ(t1.toHash(), t2.toHash());
    EXPECT_NE(t1.toHash(), t3.toHash());

    std::unordered_set<time::UTCTime, hash<time::UTCTime>> utcSet;
    utcSet.insert(t1);
    EXPECT_TRUE(utcSet.contains(t2));
    EXPECT_FALSE(utcSet.contains(t3));
}

TEST(UTCTimeTest, WeekdayCalculation) {
    const time::UTCTime christmas(2023, 12, 25, 0, 0, 0); // Monday
    EXPECT_EQ(christmas.weekday(), time::UTCTime::MONDAY);

    const time::UTCTime new_year(2000, 1, 1, 0, 0, 0); // Saturday
    EXPECT_EQ(new_year.weekday(), time::UTCTime::SATURDAY);
}

TEST(UTCTimeTest, NowFunction) {
    const auto now = time::UTCTime::now();
    EXPECT_GE(now.value(time::YEAR), 2024); // 当前年应大于等于2024
}

TEST(UTCTimeTest, LeapYearDetection) {
    EXPECT_TRUE(time::UTCTime::isLeapYear(2000));  // 400整除，闰年
    EXPECT_FALSE(time::UTCTime::isLeapYear(1900)); // 100整除但非400整除，不是闰年
    EXPECT_TRUE(time::UTCTime::isLeapYear(2024));  // 4整除，闰年
    EXPECT_FALSE(time::UTCTime::isLeapYear(2023)); // 平年
}

TEST(UTCTimeTest, DaysOfMonthCheck) {
    EXPECT_EQ(time::UTCTime::daysOfMonth(2023, 2), 28);
    EXPECT_EQ(time::UTCTime::daysOfMonth(2024, 2), 29); // 闰年
    EXPECT_EQ(time::UTCTime::daysOfMonth(2024, 1), 31);
    EXPECT_EQ(time::UTCTime::daysOfMonth(2024, 4), 30);
}

TEST(UTCTimeTest, ValidYMD) {
    EXPECT_TRUE(time::UTCTime::isValidYMD(2024, 2, 29)); // 闰年2月29合法
    EXPECT_FALSE(time::UTCTime::isValidYMD(2023, 2, 29)); // 非闰年2月29非法
    EXPECT_TRUE(time::UTCTime::isValidYMD(2024, 4, 30));  // 合法日期
    EXPECT_FALSE(time::UTCTime::isValidYMD(2024, 4, 31)); // 四月无31号
}

TEST(UTCTimeTest, ValidHMS) {
    EXPECT_TRUE(time::UTCTime::isValidHMS(0, 0, 0));
    EXPECT_TRUE(time::UTCTime::isValidHMS(23, 59, 59));
    EXPECT_FALSE(time::UTCTime::isValidHMS(24, 0, 0));
    EXPECT_FALSE(time::UTCTime::isValidHMS(0, 60, 0));
    EXPECT_FALSE(time::UTCTime::isValidHMS(0, 0, 60));
}

TEST(UTCTimeTest, ValidFullDateTime) {
    EXPECT_TRUE(time::UTCTime::isValid(2024, 2, 29, 23, 59, 59));
    EXPECT_FALSE(time::UTCTime::isValid(2023, 2, 29, 12, 0, 0)); // 非法日
    EXPECT_FALSE(time::UTCTime::isValid(2023, 12, 31, 24, 0, 0)); // 非法时
}

