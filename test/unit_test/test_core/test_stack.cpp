#include <gtest/gtest.h>
#include <stack>  // std::stack
#include "stack.h"
#include "forwardChain.h"
#include "vector.h"
#include "blocksList.h"

// Helper function to check stack equality (compare original::stack with std::stack)
template <typename T, template <typename> typename SERIAL>
bool compareStacks(const original::stack<T, SERIAL>& originalStack, const std::stack<T>& stdStack) {
    if (originalStack.size() != stdStack.size()) return false;

    original::stack<T, SERIAL> tempOriginal = originalStack;
    std::stack<T> tempStd = stdStack;

    while (!tempOriginal.empty()) {
        if (tempOriginal.pop() != tempStd.top()) {
            return false;
        }
        tempStd.pop();
    }
    return true;
}

// Test stack with `chain` as the underlying container
TEST(StackTest, ChainStack) {
    original::stack<int> s1;
    std::stack<int> s2;

    EXPECT_EQ(s1.size(), 0);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());

    // Test push, pop, and top
    s1.push(10);
    s2.push(10);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.push(20);
    s2.push(20);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    EXPECT_EQ(s1.pop(), 20);
    s2.pop();
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.pop();
    s2.pop();
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
}

// Test stack with `forwardChain` as the underlying container
TEST(StackTest, ForwardChainStack) {
    original::stack<int, original::forwardChain> s1;
    std::stack<int> s2;

    EXPECT_EQ(s1.size(), 0);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());

    // Test push, pop, and top
    s1.push(10);
    s2.push(10);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.push(20);
    s2.push(20);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    EXPECT_EQ(s1.pop(), 20);
    s2.pop();
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.pop();
    s2.pop();
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
}

// Test stack with `vector` as the underlying container
TEST(StackTest, VectorStack) {
    original::stack<int, original::vector> s1;
    std::stack<int> s2;

    EXPECT_EQ(s1.size(), 0);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());

    // Test push, pop, and top
    s1.push(10);
    s2.push(10);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.push(20);
    s2.push(20);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    EXPECT_EQ(s1.pop(), 20);
    s2.pop();
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.pop();
    s2.pop();
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
}

// Test stack with `blocksList` as the underlying container
TEST(StackTest, BlocksListStack) {
    original::stack<int, original::blocksList> s1;
    std::stack<int> s2;

    EXPECT_EQ(s1.size(), 0);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());

    // Test push, pop, and top
    s1.push(10);
    s2.push(10);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.push(20);
    s2.push(20);
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    EXPECT_EQ(s1.pop(), 20);
    s2.pop();
    EXPECT_EQ(s1.top(), s2.top());
    EXPECT_TRUE(compareStacks(s1, s2));

    s1.pop();
    s2.pop();
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
}

// Test copy constructor
TEST(StackTest, CopyConstructor) {
    original::stack<int> s1;
    s1.push(10);
    s1.push(20);

    original::stack<int> s2 = s1;
    std::stack<int> s3;
    s3.push(10);
    s3.push(20);

    EXPECT_TRUE(compareStacks(s1, s3));
    EXPECT_TRUE(compareStacks(s2, s3));
}

// Test move constructor
TEST(StackTest, MoveConstructor) {
    original::stack<int> s1;
    s1.push(10);
    s1.push(20);

    std::stack<int> s2;
    s2.push(10);
    s2.push(20);

    original::stack<int> s3 = std::move(s1);

    EXPECT_TRUE(compareStacks(s3, s2));
    EXPECT_TRUE(s1.empty());  // s1 should be empty after move
}

// Test copy assignment operator
TEST(StackTest, CopyAssignmentOperator) {
    original::stack<int> s1;
    s1.push(10);
    s1.push(20);

    original::stack<int> s2;
    s2 = s1;

    std::stack<int> s3;
    s3.push(10);
    s3.push(20);

    EXPECT_TRUE(compareStacks(s2, s3));
}

// Test move assignment operator
TEST(StackTest, MoveAssignmentOperator) {
    original::stack<int> s1;
    s1.push(10);
    s1.push(20);

    std::stack<int> s2;
    s2.push(10);
    s2.push(20);

    original::stack<int> s3;
    s3 = std::move(s1);

    EXPECT_TRUE(compareStacks(s3, s2));
    EXPECT_TRUE(s1.empty());  // s1 should be empty after move
}

