#include <algorithm>
#include <gtest/gtest.h>

#include "algorithms.h"
#include "array.h"
#include "filter.h"

namespace original {

    template<typename TYPE>
    class customFilter final : public filter<array<TYPE>> {
    protected:
        bool match(const array<TYPE> &t) const override;
    };

    template<typename TYPE>
    auto customFilter<TYPE>::match(const array<TYPE> &t) const -> bool {
        auto reversed = t;
        auto i = algorithms::reverse(reversed.first(), reversed.last());
        delete i;
        return reversed == t;
    }

    // 测试 equalFilter 类
    TEST(FilterTest, EqualFilterTest) {
        const equalFilter eqFilter(10);  // 过滤器匹配值为10的元素

        EXPECT_TRUE(eqFilter(10));  // 10 == 10 应该匹配
        EXPECT_FALSE(eqFilter(5));  // 5 != 10 不匹配
    }

    // 测试 notEqualFilter 类
    TEST(FilterTest, NotEqualFilterTest) {
        const notEqualFilter neqFilter(10);  // 过滤器匹配不等于10的元素

        EXPECT_FALSE(neqFilter(10));  // 10 == 10 不匹配
        EXPECT_TRUE(neqFilter(5));   // 5 != 10 应该匹配
    }

    // 测试 lessFilter 类
    TEST(FilterTest, LessFilterTest) {
        const lessFilter lessFilter(10);  // 过滤器匹配小于10的元素

        EXPECT_TRUE(lessFilter(5));  // 5 < 10 应该匹配
        EXPECT_FALSE(lessFilter(15)); // 15 >= 10 不匹配
    }

    // 测试 greaterFilter 类
    TEST(FilterTest, GreaterFilterTest) {
        const greaterFilter greaterFilter(10);  // 过滤器匹配大于10的元素

        EXPECT_TRUE(greaterFilter(15));  // 15 > 10 应该匹配
        EXPECT_FALSE(greaterFilter(5));  // 5 <= 10 不匹配
    }

    // 测试 notLessFilter 类
    TEST(FilterTest, NotLessFilterTest) {
        const notLessFilter notLessFilter(10);  // 过滤器匹配大于等于10的元素

        EXPECT_FALSE(notLessFilter(5));  // 5 < 10 不匹配
        EXPECT_TRUE(notLessFilter(10));  // 10 >= 10 应该匹配
        EXPECT_TRUE(notLessFilter(15));  // 15 >= 10 应该匹配
    }

    // 测试 notGreaterFilter 类
    TEST(FilterTest, NotGreaterFilterTest) {
        const notGreaterFilter notGreaterFilter(10);  // 过滤器匹配小于等于10的元素

        EXPECT_TRUE(notGreaterFilter(5));  // 5 <= 10 应该匹配
        EXPECT_TRUE(notGreaterFilter(10)); // 10 <= 10 应该匹配
        EXPECT_FALSE(notGreaterFilter(15)); // 15 > 10 不匹配
    }

    // 测试 rangeFilter 类
    TEST(FilterTest, RangeFilterTest) {
        const rangeFilter rangeFilter(5, 10);  // 过滤器匹配5到10之间的元素

        EXPECT_TRUE(rangeFilter(5));   // 5 在区间 [5, 10] 内，应该匹配
        EXPECT_TRUE(rangeFilter(10));  // 10 在区间 [5, 10] 内，应该匹配
        EXPECT_FALSE(rangeFilter(3));  // 3 < 5 不在区间内，不匹配
        EXPECT_FALSE(rangeFilter(15)); // 15 > 10 不在区间内，不匹配
    }

    // 测试 clone 方法
    TEST(FilterTest, CloneTest) {
        // 测试 filter 基类的 clone 方法
        const equalFilter eqFilter(10);
        const auto clonedFilter = eqFilter.clone();

        EXPECT_TRUE(eqFilter(10));    // 原始过滤器匹配 10
        EXPECT_TRUE(clonedFilter->operator()(10));  // 克隆过滤器也匹配 10

        delete clonedFilter;  // 清理克隆的过滤器
    }

    // 测试自定义过滤器（扩展基类）
    TEST(FilterTest, CustomFilterTest) {
        const customFilter<int> f;
        const array arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        EXPECT_FALSE(f(arr));
    }

}
