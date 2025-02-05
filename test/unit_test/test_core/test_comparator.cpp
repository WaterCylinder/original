#include <numeric>
#include <gtest/gtest.h>

#include "array.h"
#include "comparator.h"

namespace original {

    template<typename TYPE>
    class customComparator final : public comparator<array<TYPE>>{
    public:
        bool compare(const array<TYPE> &t1, const array<TYPE> &t2) const override;
    };

    template<typename TYPE>
    bool customComparator<TYPE>::compare(const array<TYPE> &t1, const array<TYPE> &t2) const {
        return std::accumulate(t1.begin(), t1.end(), TYPE{}, std::plus<TYPE>())
             < std::accumulate(t2.begin(), t2.end(), TYPE{}, std::plus<TYPE>());
    }

    // 测试 increaseComparator
    TEST(ComparatorTest, IncreaseComparator) {
        const increaseComparator<int> comp;
        EXPECT_TRUE(comp(3, 5));  // 3 < 5
        EXPECT_FALSE(comp(5, 3)); // 5 is not less than 3
        EXPECT_FALSE(comp(5, 5)); // 5 is not less than 5
    }

    // 测试 decreaseComparator
    TEST(ComparatorTest, DecreaseComparator) {
        const decreaseComparator<int> comp;
        EXPECT_TRUE(comp(5, 3));  // 5 > 3
        EXPECT_FALSE(comp(3, 5)); // 3 is not greater than 5
        EXPECT_FALSE(comp(5, 5)); // 5 is not greater than 5
    }

    // 测试 equalComparator
    TEST(ComparatorTest, EqualComparator) {
        const equalComparator<int> comp;
        EXPECT_TRUE(comp(5, 5));  // 5 == 5
        EXPECT_FALSE(comp(5, 3)); // 5 != 3
        EXPECT_FALSE(comp(3, 5)); // 3 != 5
    }

    // 测试 notEqualComparator
    TEST(ComparatorTest, NotEqualComparator) {
        const notEqualComparator<int> comp;
        EXPECT_TRUE(comp(5, 3));  // 5 != 3
        EXPECT_FALSE(comp(5, 5)); // 5 == 5
        EXPECT_FALSE(comp(3, 3)); // 3 == 3
    }

    // 测试 increaseNotStrictComparator
    TEST(ComparatorTest, IncreaseNotStrictComparator) {
        const increaseNotStrictComparator<int> comp;
        EXPECT_TRUE(comp(3, 5));  // 3 < 5
        EXPECT_TRUE(comp(5, 5));  // 5 == 5
        EXPECT_FALSE(comp(5, 3)); // 5 is not less than or equal to 3
    }

    // 测试 decreaseNotStrictComparator
    TEST(ComparatorTest, DecreaseNotStrictComparator) {
        const decreaseNotStrictComparator<int> comp;
        EXPECT_TRUE(comp(5, 3));  // 5 > 3
        EXPECT_TRUE(comp(5, 5));  // 5 == 5
        EXPECT_FALSE(comp(3, 5)); // 3 is not greater than or equal to 5
    }

    // 测试 customComparator 的行为
    TEST(ComparatorTest, CustomComparatorTest) {
        const customComparator<int> comp;

        const array arr1 = {1, 2, 3};
        const array arr2 = {4, 5, 6};
        const array arr3 = {1, 1, 1};

        // arr1 总和 6 < arr2 总和 15
        EXPECT_TRUE(comp(arr1, arr2));
        // arr2 总和 15 > arr3 总和 3
        EXPECT_FALSE(comp(arr2, arr3));
        // arr3 总和 3 < arr1 总和 6
        EXPECT_TRUE(comp(arr3, arr1));

        // 测试相同总和
        const array arr4 = {2, 2, 2};
        EXPECT_FALSE(comp(arr1, arr4));  // arr1 总和为6, arr4 总和为6
        EXPECT_FALSE(comp(arr1, arr1)); // arr1的总和等于自己
    }
}


