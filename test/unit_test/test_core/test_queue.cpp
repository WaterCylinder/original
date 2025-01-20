#include <gtest/gtest.h>
#include <queue>  // std::queue
#include "queue.h"
#include "forwardChain.h"
#include "vector.h"
#include "blocksList.h"

// Helper function to compare two queues (original::queue and std::queue)
template <typename T, template <typename> typename SERIAL>
bool compareQueues(const original::queue<T, SERIAL>& originalQueue, const std::queue<T>& stdQueue) {
    if (originalQueue.size() != stdQueue.size()) return false;

    original::queue<T, SERIAL> tempOriginal = originalQueue;
    std::queue<T> tempStd = stdQueue;

    while (!tempOriginal.empty()) {
        if (tempOriginal.head() != tempStd.front()) {
            return false;
        }
        tempOriginal.pop();
        tempStd.pop();
    }
    return true;
}

// Test queue with `chain` as the underlying container
TEST(QueueTest, ChainQueue) {
    original::queue<int> q1;
    std::queue<int> q2;

    EXPECT_EQ(q1.size(), 0);
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());

    // Test push, head, tail, pop
    q1.push(10);
    q2.push(10);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.push(20);
    q2.push(20);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    EXPECT_EQ(q1.pop(), 10);
    q2.pop();
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.pop();
    q2.pop();
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());
}

// Test queue with `forwardChain` as the underlying container
TEST(QueueTest, ForwardChainQueue) {
    original::queue<int, original::forwardChain> q1;
    std::queue<int> q2;

    EXPECT_EQ(q1.size(), 0);
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());

    // Test push, head, tail, pop
    q1.push(10);
    q2.push(10);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.push(20);
    q2.push(20);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    EXPECT_EQ(q1.pop(), 10);
    q2.pop();
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.pop();
    q2.pop();
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());
}

// Test queue with `vector` as the underlying container
TEST(QueueTest, VectorQueue) {
    original::queue<int, original::vector> q1;
    std::queue<int> q2;

    EXPECT_EQ(q1.size(), 0);
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());

    // Test push, head, tail, pop
    q1.push(10);
    q2.push(10);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.push(20);
    q2.push(20);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    EXPECT_EQ(q1.pop(), 10);
    q2.pop();
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.pop();
    q2.pop();
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());
}

// Test queue with `blocksList` as the underlying container
TEST(QueueTest, BlocksListQueue) {
    original::queue<int, original::blocksList> q1;
    std::queue<int> q2;

    EXPECT_EQ(q1.size(), 0);
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());

    // Test push, head, tail, pop
    q1.push(10);
    q2.push(10);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.push(20);
    q2.push(20);
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    EXPECT_EQ(q1.pop(), 10);
    q2.pop();
    EXPECT_EQ(q1.head(), q2.front());
    EXPECT_EQ(q1.tail(), q2.back());
    EXPECT_TRUE(compareQueues(q1, q2));

    q1.pop();
    q2.pop();
    EXPECT_TRUE(q1.empty());
    EXPECT_TRUE(q2.empty());
}

// Test copy constructor
TEST(QueueTest, CopyConstructor) {
    original::queue<int> q1;
    q1.push(10);
    q1.push(20);

    original::queue<int> q2 = q1;
    std::queue<int> q3;
    q3.push(10);
    q3.push(20);

    EXPECT_TRUE(compareQueues(q1, q3));
    EXPECT_TRUE(compareQueues(q2, q3));
}

// Test move constructor
TEST(QueueTest, MoveConstructor) {
    original::queue<int> q1;
    q1.push(10);
    q1.push(20);

    std::queue<int> q2;
    q2.push(10);
    q2.push(20);

    original::queue<int> q3 = std::move(q1);

    EXPECT_TRUE(compareQueues(q3, q2));
    EXPECT_TRUE(q1.empty());  // q1 should be empty after move
}

// Test copy assignment operator
TEST(QueueTest, CopyAssignmentOperator) {
    original::queue<int> q1;
    q1.push(10);
    q1.push(20);

    original::queue<int> q2;
    EXPECT_TRUE(q2.empty());
    q2 = q1;

    std::queue<int> q3;
    q3.push(10);
    q3.push(20);

    EXPECT_TRUE(compareQueues(q2, q3));
}

// Test move assignment operator
TEST(QueueTest, MoveAssignmentOperator) {
    original::queue<int> q1;
    q1.push(10);
    q1.push(20);

    std::queue<int> q2;
    q2.push(10);
    q2.push(20);

    original::queue<int> q3;
    q3 = std::move(q1);

    EXPECT_TRUE(compareQueues(q3, q2));
    EXPECT_TRUE(q1.empty());  // q1 should be empty after move
}