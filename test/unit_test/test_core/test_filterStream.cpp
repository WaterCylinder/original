#include <gtest/gtest.h>
#include "filterStream.h"
#include "vector.h"

namespace original {

    TEST(FilterStreamTest, AndOptTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const vector vec1 = {1, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;
        constexpr int val3 = 3;
        const lessFilter less(val2);
        const greaterFilter greater(val1);
        const notEqualFilter n_equal(val3);
        const filterStream fs1 = less && greater;
        const filterStream fs2 = fs1 && n_equal;
        const filterStream fs3 = n_equal && fs1;
        vec.forEach(
            [=](const int x) {
                ASSERT_EQ(fs1(x), x < val2 && x > val1);
                ASSERT_EQ(fs2(x), x < val2 && x > val1 && x != val3);
                ASSERT_EQ(fs3(x), x < val2 && x > val1 && x != val3);
            }
        );
    }

    TEST(FilterStreamTest, OrOptTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const vector vec1 = {1, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;
        constexpr int val3 = 3;
        const lessFilter less(val1);
        const greaterFilter greater(val2);
        const equalFilter equal(val3);
        const filterStream fs1 = less || greater;
        const filterStream fs2 = fs1 || equal;
        const filterStream fs3 = equal || fs1;
        vec.forEach(
            [=](const int x) {
                ASSERT_EQ(fs1(x), x < val1 || x > val2);
                ASSERT_EQ(fs2(x), x < val1 || x > val2 || x == val3);
                ASSERT_EQ(fs3(x), x < val1 || x > val2 || x == val3);
            }
        );
    }

    TEST(FilterStresmTest, NotOptTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 3;
        const equalFilter equal(val1);
        const filterStream fs = !equal;
        vec.forEach(
            [=](const int x) {
                ASSERT_EQ(fs(x), x != val1);
            }
        );
    }

    // 测试 AND 操作符组合与 group 使用
    TEST(FilterStreamTest, AndOptWithGroupTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;
        constexpr int val3 = 3;

        const lessFilter less(val2);
        const greaterFilter greater(val1);
        const notEqualFilter n_equal(val3);

        // 使用 group 将条件括起来
        const filterStream fs1 = group(less && greater);
        const filterStream fs2 = group(fs1 && n_equal);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs1(x), x < val2 && x > val1);
            ASSERT_EQ(fs2(x), x < val2 && x > val1 && x != val3);
        });
    }

    // 测试 OR 操作符组合与 group 使用
    TEST(FilterStreamTest, OrOptWithGroupTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;
        constexpr int val3 = 3;

        const lessFilter less(val1);
        const greaterFilter greater(val2);
        const equalFilter equal(val3);

        // 使用 group 将条件括起来
        const filterStream fs1 = group(less || greater);
        const filterStream fs2 = group(fs1 || equal);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs1(x), x < val1 || x > val2);
            ASSERT_EQ(fs2(x), x < val1 || x > val2 || x == val3);
        });
    }

    // 测试 NOT 操作符组合与 group 使用
    TEST(FilterStreamTest, NotOptWithGroupTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 3;
        const equalFilter equal(val1);

        // 使用 group 将条件括起来
        const filterStream fs = group(!equal);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs(x), x != val1);
        });
    }

    // 测试 AND、OR、NOT 操作符混合与 group 使用
    TEST(FilterStreamTest, MixedOperatorsWithGroupTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;
        constexpr int val3 = 3;

        const lessFilter less(val2);
        const greaterFilter greater(val1);
        const notEqualFilter n_equal(val3);

        // 使用 group 进行复杂组合
        const filterStream fs = group(group(less && greater) || n_equal);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs(x), (x < val2 && x > val1) || x != val3);
        });
    }

    // 测试 AND、OR、NOT 操作符组合以及 group 操作符的混合使用
    TEST(FilterStreamTest, GroupWithMultipleConditionsTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 4;
        constexpr int val3 = 3;

        const lessFilter less(val2);
        const greaterFilter greater(val1);
        const equalFilter equal(val3);

        // 使用 group 对多个操作符组合进行括号操作
        const filterStream fs = group(less && greater || equal);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs(x), (x < val2 && x > val1) || x == val3);
        });
    }

    // 测试不同组合的 group 和 AND、OR 操作符
    TEST(FilterStreamTest, GroupAndOrCombinationTest) {
        const vector vec = {1, 2, 3, 4, 5};

        constexpr int val1 = 1;
        constexpr int val2 = 5;

        const lessFilter less(val1);
        const greaterFilter greater(val2);

        // 组合 AND 和 OR 操作符以及 group
        const filterStream fs = group(group(less || greater) && greater);

        vec.forEach([=](const int x) {
            ASSERT_EQ(fs(x), (x < val1 || x > val2) && x > val2);
        });
    }

}
