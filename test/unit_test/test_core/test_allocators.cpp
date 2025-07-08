#include "gtest/gtest.h"
#include "allocator.h"
#include "blocksList.h"
#include "chain.h"
#include "forwardChain.h"
#include "vector.h"

// Test fixture for allocator tests
class AllocatorTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Typed test suite for different container types
template <typename T>
class ContainerAllocatorTypedTest : public AllocatorTest {
protected:
    using ContainerType = T;

    void TestBasicOperations() {
        ContainerType container;

        // Test push/pop operations
        container.pushEnd(1);
        container.pushEnd(2);
        container.pushBegin(0);
        container.push(2, 1); // Insert at middle

        EXPECT_EQ(container.size(), 4);
        EXPECT_EQ(container.get(0), 0);
        EXPECT_EQ(container.get(1), 1);
        EXPECT_EQ(container.get(2), 1);
        EXPECT_EQ(container.get(3), 2);

        // Test element access
        container[1] = 5;
        EXPECT_EQ(container.get(1), 5);

        // Test removal
        EXPECT_EQ(container.popBegin(), 0);
        EXPECT_EQ(container.popEnd(), 2);
        EXPECT_EQ(container.pop(1), 1);
        EXPECT_EQ(container.size(), 1);
    }

    void TestLargeContainer() {
        constexpr int count = 10000;
        ContainerType container;

        for (int i = 0; i < count; ++i) {
            container.pushEnd(i);
        }

        EXPECT_EQ(container.size(), count);

        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(container.get(i), i);
        }

        while (!container.empty()) {
            container.popEnd();
        }

        EXPECT_EQ(container.size(), 0);
    }

    void TestCopyAndMove() {
        ContainerType container1;
        container1.pushEnd(1);
        container1.pushEnd(2);
        container1.pushEnd(3);

        // Test copy constructor
        ContainerType container2(container1);
        EXPECT_EQ(container2.size(), 3);
        EXPECT_EQ(container2.get(0), 1);
        EXPECT_EQ(container2.get(1), 2);
        EXPECT_EQ(container2.get(2), 3);

        // Test move constructor
        ContainerType container3(std::move(container1));
        EXPECT_EQ(container3.size(), 3);
        EXPECT_EQ(container1.size(), 0); // NOLINT

        // Test copy assignment
        ContainerType container4;
        container4 = container2;
        EXPECT_EQ(container4.size(), 3);

        // Test move assignment
        ContainerType container5;
        container5 = std::move(container2);
        EXPECT_EQ(container5.size(), 3);
        EXPECT_EQ(container2.size(), 0); // NOLINT
    }
};

using ContainerTypes = testing::Types<
    original::blocksList<int>,
    original::blocksList<int, original::objPoolAllocator<int>>,
    original::chain<int>,
    original::chain<int, original::objPoolAllocator<int>>,
    original::forwardChain<int>,
    original::forwardChain<int, original::objPoolAllocator<int>>,
    original::vector<int>,
    original::vector<int, original::objPoolAllocator<int>>
>;

TYPED_TEST_SUITE(ContainerAllocatorTypedTest, ContainerTypes);

TYPED_TEST(ContainerAllocatorTypedTest, BasicOperations) {
    this->TestBasicOperations();
}

TYPED_TEST(ContainerAllocatorTypedTest, LargeContainer) {
    this->TestLargeContainer();
}

TYPED_TEST(ContainerAllocatorTypedTest, CopyAndMove) {
    this->TestCopyAndMove();
}

// Test the allocators utility class directly
TEST(AllocatorsTest, MallocFree) {
    int* intArray = original::allocators::malloc<int>(10);
    ASSERT_NE(intArray, nullptr);
    original::allocators::free(intArray);

    int* nullArray = original::allocators::malloc<int>(0);
    ASSERT_EQ(nullArray, nullptr);
    original::allocators::free(nullArray);
}

TEST(AllocatorTest, BasicOperations) {
    original::allocator<int> alloc;

    int* arr = alloc.allocate(5);
    ASSERT_NE(arr, nullptr);

    for (int i = 0; i < 5; ++i) {
        alloc.construct(&arr[i], i);
        EXPECT_EQ(arr[i], i);
    }

    for (int i = 0; i < 5; ++i) {
        original::allocator<int>::destroy(&arr[i]);
    }

    alloc.deallocate(arr, 5);
}