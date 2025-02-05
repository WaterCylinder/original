#include <gtest/gtest.h>
#include "array.h"
#include "vector.h"
#include "algorithms.h"

namespace original {

    // 测试 distance 方法
    TEST(AlgorithmsTest, DistanceTest) {
        const array arr = {1, 2, 3, 4, 5};
        const auto beginIt = arr.begins();
        const auto endIt = arr.ends();
        EXPECT_EQ(algorithms::distance(endIt, beginIt), 4);
        delete beginIt;
        delete endIt;
    }

    // 测试 frontOf 方法
    TEST(AlgorithmsTest, FrontOfTest) {
        const array arr = {1, 2, 3, 4, 5};
        const auto beginIt = arr.begins();
        const auto newIt = algorithms::frontOf(beginIt, 2);
        EXPECT_EQ(newIt->get(), 3);
        delete beginIt;
        delete newIt;
    }

    // 测试 backOf 方法
    TEST(AlgorithmsTest, BackOfTest) {
        const array arr = {1, 2, 3, 4, 5};
        const auto endIt = arr.ends();
        const auto newIt = algorithms::backOf(endIt, 1);
        EXPECT_EQ(newIt->get(), 4);
        delete endIt;
        delete newIt;
    }

    // 测试 allOf 方法
    TEST(AlgorithmsTest, AllOfTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const greaterFilter greaterThan2(2);
        EXPECT_FALSE(algorithms::allOf(vec.first(), vec.last(), greaterThan2));
    }

    // 测试 anyOf 方法
    TEST(AlgorithmsTest, AnyOfTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const greaterFilter greaterThan4(4);
        EXPECT_TRUE(algorithms::anyOf(vec.first(), vec.last(), greaterThan4));
    }

    // 测试 noneOf 方法
    TEST(AlgorithmsTest, NoneOfTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const lessFilter lessThan1(1);
        EXPECT_TRUE(algorithms::noneOf(vec.first(), vec.last(), lessThan1));
    }

    // 测试 find 方法（通过值查找）
    TEST(AlgorithmsTest, FindTest) {
        const vector vec = {1, 2, 3, 4, 5};
        const auto result = algorithms::find(vec.first(), vec.last(), 3);
        EXPECT_EQ(result->get(), 3);
        delete result;
    }

    // 测试 count 方法（计数元素出现次数）
    TEST(AlgorithmsTest, CountTest) {
        const vector vec = {1, 2, 3, 3, 4, 5};
        EXPECT_EQ(algorithms::count(vec.first(), vec.last(), 3), 2);
    }

    // 测试 equal 方法
    TEST(AlgorithmsTest, EqualTest) {
        const vector vec1 = {1, 2, 3, 4, 5};
        const vector vec2 = {1, 2, 3, 4, 5};
        EXPECT_TRUE(algorithms::equal(vec1.first(), vec1.last(), vec2.first(), vec2.last()));

        const vector vec3 = {1, 2, 3, 4};
        EXPECT_FALSE(algorithms::equal(vec1.first(), vec1.last(), vec3.first(), vec3.last()));
    }

    // 测试 forEach 方法（执行操作）
    TEST(AlgorithmsTest, ForEachTest) {
        const array arr = {1, 2, 3, 4, 5};
        int sum = 0;
        auto acc = [&sum](const int val) {
            sum += val;
        };
        algorithms::forEach(arr.first(), arr.last(), acc);
        EXPECT_EQ(sum, 15);
    }

    // 测试 fill 方法（填充值）
    TEST(AlgorithmsTest, FillTest) {
        const array arr = {1, 2, 3, 4, 5};
        algorithms::fill(arr.first(), arr.last(), 10);
        for (const auto it = arr.begin(); it != arr.end(); it.next()) {
            EXPECT_EQ(it.get(), 10);
        }
    }

    // 测试 swap 方法（交换元素）
    TEST(AlgorithmsTest, SwapTest) {
        const array arr = {1, 2, 3, 4, 5};
        const auto it1 = arr.begins();
        const auto it2 = arr.begins();
        it2->next();
        algorithms::swap(it1, it2);
        EXPECT_EQ(it1->get(), 2);
        EXPECT_EQ(it2->get(), 1);
        delete it1;
        delete it2;
    }

    // 测试 copy 方法（拷贝元素）
    TEST(AlgorithmsTest, CopyTest) {
        const array arr1 = {1, 2, 3, 4, 5};
        const array arr2 = {0, 0, 0, 0, 0};
        algorithms::copy(arr1.first(), arr1.last(), arr2.first());
        for (auto it = arr1.begin(); it != arr1.end(); it.next()) {
            EXPECT_EQ(it.get(), arr2.get(it - arr1.begin()));
        }
    }

    // 测试 reverse 方法（反转元素）
    TEST(AlgorithmsTest, ReverseTest) {
        const array arr = {1, 2, 3, 4, 5};
        algorithms::reverse(arr.first(), arr.last());
        const int expected[] = {5, 4, 3, 2, 1};
        int idx = 0;
        for (const auto it = arr.begin(); it != arr.end(); it.next()) {
            EXPECT_EQ(it.get(), expected[idx++]);
        }
    }
}
