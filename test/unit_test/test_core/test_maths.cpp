#include <gtest/gtest.h>
#include "maths.h"
#include <cmath>

using namespace original;

// Tests for abs
TEST(MathsTests, AbsIntPositive) {
    EXPECT_EQ(original::abs(5), std::abs(5));
}

TEST(MathsTests, AbsIntNegative) {
    EXPECT_EQ(original::abs(-5), std::abs(-5));
}

TEST(MathsTests, AbsDoublePositive) {
    EXPECT_EQ(original::abs(5.5), std::abs(5.5));
}

TEST(MathsTests, AbsDoubleNegative) {
    EXPECT_EQ(original::abs(-5.5), std::abs(-5.5));
}

// Tests for max
TEST(MathsTests, MaxInt) {
    EXPECT_EQ(original::max(3, 5), std::max(3, 5));
}

TEST(MathsTests, MaxDouble) {
    EXPECT_EQ(original::max(3.5, 5.5), std::max(3.5, 5.5));
}

TEST(MathsTests, MaxEqual) {
    EXPECT_EQ(original::max(3, 3), std::max(3, 3));
}

// Tests for min
TEST(MathsTests, MinInt) {
    EXPECT_EQ(original::min(3, 5), std::min(3, 5));
}

TEST(MathsTests, MinDouble) {
    EXPECT_EQ(original::min(3.5, 5.5), std::min(3.5, 5.5));
}

TEST(MathsTests, MinEqual) {
    EXPECT_EQ(original::min(3, 3), std::min(3, 3));
}

// Tests for pow
TEST(MathsTests, PowPositiveExp) {
    EXPECT_EQ(original::pow(2.0, 3), std::pow(2.0, 3));
}

TEST(MathsTests, PowNegativeExp) {
    EXPECT_EQ(original::pow(2.0, -3), std::pow(2.0, -3));
}

TEST(MathsTests, PowZeroBase) {
    EXPECT_EQ(original::pow(0.0, 5), std::pow(0.0, 5));
}

TEST(MathsTests, PowZeroExp) {
    EXPECT_EQ(original::pow(2.0, 0), std::pow(2.0, 0));
}

TEST(MathsTests, PowZeroBaseZeroExp) {
    EXPECT_THROW(original::pow(0.0, 0), valueError);
}
