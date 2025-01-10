#include <gtest/gtest.h>
#include <array>  // std::array
#include "array.h"  // original::array

namespace original {
    template <typename TYPE, std::size_t N>
    bool compareArrays(const array<TYPE>& a, const std::array<TYPE, N>& b) {
        if (a.size() != N) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (a.get(i) != b[i]) return false;
        }
        return true;
    }

class ArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initial setup before each test
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

TEST(ArrayTest, ConstructorAndSize) {
    original::array<int> arr1(10);  // size = 10
    std::array<int, 10> stdArr1{};
    EXPECT_EQ(arr1.size(), stdArr1.size());

    original::array<int> arr2 = {1, 2, 3, 4, 5};
    std::array<int, 5> stdArr2 = {1, 2, 3, 4, 5};
    EXPECT_TRUE(original::compareArrays(arr2, stdArr2));

    original::array<int> arr3;
    std::array<int, 0> stdArr3{};
    EXPECT_EQ(arr3.size(), stdArr3.size());
}

TEST(ArrayTest, IndexingAndDataAccess) {
    original::array<int> arr = {1, 2, 3, 4, 5};
    std::array<int, 5> stdArr = {1, 2, 3, 4, 5};

    EXPECT_EQ(arr[0], stdArr[0]);
    EXPECT_EQ(arr[4], stdArr[4]);

    arr.set(2, 100);
    stdArr[2] = 100;
    EXPECT_EQ(arr[2], stdArr[2]);
    EXPECT_TRUE(original::compareArrays(arr, stdArr));

    EXPECT_THROW(arr.get(10), outOfBoundError);
    EXPECT_THROW(arr[10], outOfBoundError);
}

TEST(ArrayTest, CopyConstructorAndAssignmentOperator) {
    original::array<int> arr1 = {1, 2, 3, 4, 5};
    original::array<int> arr2 = arr1;  // Copy constructor
    std::array<int, 5> stdArr = {1, 2, 3, 4, 5};

    EXPECT_TRUE(original::compareArrays(arr1, stdArr));
    EXPECT_TRUE(original::compareArrays(arr2, stdArr));

    original::array<int> arr3;
    arr3 = arr1;  // Assignment operator
    EXPECT_TRUE(original::compareArrays(arr3, stdArr));
}

TEST(ArrayTest, EqualityOperator) {
    original::array<int> arr1 = {1, 2, 3, 4, 5};
    original::array<int> arr2 = {1, 2, 3, 4, 5};
    original::array<int> arr3 = {5, 4, 3, 2, 1};

    EXPECT_TRUE(arr1 == arr2);
    EXPECT_FALSE(arr1 == arr3);
}

TEST(ArrayTest, IndexOf) {
    original::array<int> arr = {10, 20, 30, 40, 50};

    EXPECT_EQ(arr.indexOf(30), 2);
    EXPECT_EQ(arr.indexOf(100), arr.size());
}

TEST(ArrayTest, Iterator) {
    original::array<int> arr = {10, 20, 30, 40, 50};

    auto beginIt = arr.begins();
    EXPECT_EQ(beginIt->className(), "array::Iterator");
    auto endIt = arr.ends();
    EXPECT_EQ(endIt->className(), "array::Iterator");
    EXPECT_GE(*endIt - *beginIt, 0);
    EXPECT_LE(*beginIt - *endIt, 0);


    delete beginIt;
    delete endIt;
}

TEST(ArrayTest, CloneIterator) {
    original::array<int> arr = {10, 20, 30, 40, 50};

    auto it = arr.begins();
    auto clonedIt = it->clone();
    EXPECT_EQ(clonedIt->className(), "array::Iterator");
    EXPECT_EQ(it->atNext(arr.ends()), clonedIt->atNext(arr.ends()));

    delete it;
    delete clonedIt;
}

TEST(ArrayTest, ClassName) {
    original::array<int> arr;
    EXPECT_EQ(arr.className(), "array");
}

TEST(ArrayTest, Destruction) {
    // Test if the array is destructed properly
    {
        original::array<int> arr = {1, 2, 3};
    } // arr goes out of scope here
    EXPECT_TRUE(true);  // No crash or memory leak
}

TEST(ArrayTest, CopyAndMoveSemantics) {
    original::array<int> arr1 = {1, 2, 3, 4, 5};
    auto arr2 = std::move(arr1);  // Move constructor
    EXPECT_EQ(arr2.size(), 5);
    EXPECT_EQ(arr1.size(), 0);
    EXPECT_THROW(arr1[0], outOfBoundError);  // arr1 should be in an invalid state

    original::array<int> arr3 = {10, 20, 30};
    arr3 = std::move(arr2);  // Move assignment
    EXPECT_EQ(arr3.size(), 5);
    EXPECT_THROW(arr2[0], outOfBoundError);  // arr2 should be in an invalid state
}

}  // namespace original
