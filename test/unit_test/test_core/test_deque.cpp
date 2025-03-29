#include <gtest/gtest.h>
#include <deque>
#include "blocksList.h"
#include "deque.h"
#include "forwardChain.h"
#include "vector.h"

// Helper function to compare two deques (original::deque and std::deque)
template <typename T, template <typename, typename> typename SERIAL>
bool compareDeques(const original::deque<T, SERIAL>& originalDeque, const std::deque<T>& stdDeque) {
    if (originalDeque.size() != stdDeque.size()) return false;

    original::deque<T, SERIAL> tempOriginal = originalDeque;
    std::deque<T> tempStd = stdDeque;

    while (!tempOriginal.empty()) {
        if (tempOriginal.head() != tempStd.front()) {
            return false;
        }
        tempOriginal.popBegin();
        tempStd.pop_front();
    }
    return true;
}

// Test deque with `chain` as the underlying container
TEST(DequeTest, ChainDeque) {
    original::deque<int> d1;
    std::deque<int> d2;

    EXPECT_EQ(d1.size(), 0);
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());

    // Test pushBegin, pushEnd, head, tail, popBegin, popEnd
    d1.pushBegin(10);
    d2.push_front(10);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    d1.pushEnd(20);
    d2.push_back(20);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popBegin(), 10);
    d2.pop_front();
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popEnd(), 20);
    d2.pop_back();
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());
}

// Test deque with `forwardChain` as the underlying container
TEST(DequeTest, ForwardChainDeque) {
    original::deque<int, original::forwardChain> d1;
    std::deque<int> d2;

    EXPECT_EQ(d1.size(), 0);
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());

    // Test pushBegin, pushEnd, head, tail, popBegin, popEnd
    d1.pushBegin(10);
    d2.push_front(10);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    d1.pushEnd(20);
    d2.push_back(20);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popBegin(), 10);
    d2.pop_front();
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popEnd(), 20);
    d2.pop_back();
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());
}

// Test deque with `vector` as the underlying container
TEST(DequeTest, VectorDeque) {
    original::deque<int, original::vector> d1;
    std::deque<int> d2;

    EXPECT_EQ(d1.size(), 0);
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());

    // Test pushBegin, pushEnd, head, tail, popBegin, popEnd
    d1.pushBegin(10);
    d2.push_front(10);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    d1.pushEnd(20);
    d2.push_back(20);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popBegin(), 10);
    d2.pop_front();
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popEnd(), 20);
    d2.pop_back();
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());
}

// Test deque with `blocksList` as the underlying container
TEST(DequeTest, BlocksListDeque) {
    original::deque<int, original::blocksList> d1;
    std::deque<int> d2;

    EXPECT_EQ(d1.size(), 0);
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());

    // Test pushBegin, pushEnd, head, tail, popBegin, popEnd
    d1.pushBegin(10);
    d2.push_front(10);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    d1.pushEnd(20);
    d2.push_back(20);
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popBegin(), 10);
    d2.pop_front();
    EXPECT_EQ(d1.head(), d2.front());
    EXPECT_EQ(d1.tail(), d2.back());
    EXPECT_TRUE(compareDeques(d1, d2));

    EXPECT_EQ(d1.popEnd(), 20);
    d2.pop_back();
    EXPECT_TRUE(d1.empty());
    EXPECT_TRUE(d2.empty());
}

// Test copy constructor
TEST(DequeTest, CopyConstructor) {
    original::deque<int> d1;
    d1.pushBegin(10);
    d1.pushEnd(20);

    original::deque<int> d2 = d1;
    std::deque<int> d3;
    d3.push_front(10);
    d3.push_back(20);

    EXPECT_TRUE(compareDeques(d1, d3));
    EXPECT_TRUE(compareDeques(d2, d3));
}

// Test move constructor
TEST(DequeTest, MoveConstructor) {
    original::deque<int> d1;
    d1.pushBegin(10);
    d1.pushEnd(20);

    std::deque<int> d2;
    d2.push_front(10);
    d2.push_back(20);

    original::deque<int> d3 = std::move(d1);

    EXPECT_TRUE(compareDeques(d3, d2));
    EXPECT_TRUE(d1.empty());  // d1 should be empty after move
}

// Test copy assignment operator
TEST(DequeTest, CopyAssignmentOperator) {
    original::deque<int> d1;
    d1.pushBegin(10);
    d1.pushEnd(20);

    original::deque<int> d2;
    d2 = d1;

    std::deque<int> d3;
    d3.push_front(10);
    d3.push_back(20);

    EXPECT_TRUE(compareDeques(d2, d3));
}

// Test move assignment operator
TEST(DequeTest, MoveAssignmentOperator) {
    original::deque<int> d1;
    d1.pushBegin(10);
    d1.pushEnd(20);

    std::deque<int> d2;
    d2.push_front(10);
    d2.push_back(20);

    original::deque<int> d3;
    d3 = std::move(d1);

    EXPECT_TRUE(compareDeques(d3, d2));
    EXPECT_TRUE(d1.empty());  // d1 should be empty after move
}
