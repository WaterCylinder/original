#include <gtest/gtest.h>
#include "coroutines.h"

using namespace original;

TEST(CoroutineGeneratorTest, SimpleRange) {
    auto gen = coroutine::rangesOf<integer>(0, 5);
    std::vector<integer> result;
    while (auto val = gen.next()) {
        result.push_back(*val);
    }
    EXPECT_EQ((std::vector<integer>{0, 1, 2, 3, 4}), result);
}

TEST(CoroutineGeneratorTest, StepRange) {
    auto gen = coroutine::rangesOf(0, 10, 2);
    std::vector<int> result;
    while (auto val = gen.next()) {
        result.push_back(*val);
    }
    EXPECT_EQ((std::vector{0, 2, 4, 6, 8}), result);
}

TEST(CoroutineGeneratorTest, NegativeStepRange) {
    auto gen = coroutine::rangesOf(5, -1, -2);
    std::vector<int> result;
    while (auto val = gen.next()) {
        result.push_back(*val);
    }
    EXPECT_EQ((std::vector{5, 3, 1}), result);
}

TEST(CoroutineGeneratorTest, PeekDoesNotAdvance) {
    auto gen = coroutine::rangesOf(0, 3);
    EXPECT_FALSE(gen.peek()); // peek first
    if (auto val = gen.next()) {
        EXPECT_EQ(0, *val); // next returns same value
    }
    if (auto val = gen.peek()) {
        EXPECT_EQ(0, *val); // peek moves with next
    }
    if (auto val = gen.next()) {
        EXPECT_EQ(1, *val);
    }
}

TEST(CoroutineGeneratorTest, EmptyWhenInvalidStep) {
    auto gen = coroutine::rangesOf(0, 10, 0); // difference == 0
    EXPECT_FALSE(gen.next());
}

TEST(CoroutineGeneratorTest, EmptyWhenStartBeyondEnd) {
    auto gen = coroutine::rangesOf(10, 0, 1); // start > end
    EXPECT_FALSE(gen.next());
}

TEST(CoroutineGeneratorTest, IteratorTraverse){
    auto gen = coroutine::rangesOf<integer>(0, 10);
    std::vector<integer> result;
    for (auto it = gen.begin();it != coroutine::generator<long>::end(); ++it)
    {
        result.push_back(*it);
    }
    EXPECT_EQ((std::vector<integer>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), result);
}

TEST(CoroutineGeneratorTest, IteratorRangedFor){
    auto gen = coroutine::rangesOf<integer>(0, 10);
    std::vector<integer> result;
    for (const auto& elem : gen)
    {
        result.push_back(elem);
    }
    EXPECT_EQ((std::vector<integer>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), result);

    auto gen2 = coroutine::rangesOf(10, 0, -1);
    std::vector<int> result2;
    for (const auto& elem : gen2)
    {
        result2.push_back(elem);
    }
    EXPECT_EQ((std::vector{10, 9, 8, 7, 6, 5, 4, 3, 2, 1}), result2);
}

TEST(CoroutineGeneratorTest, IteratorEdgeCase) {
    auto g = coroutine::rangesOf(0, 10, 0); // 步长为0
    EXPECT_EQ(g.begin(), g.end());

    auto g2 = coroutine::rangesOf(0, 10);
    auto end_it = g2.end();
    EXPECT_THROW(*end_it, nullPointerError);
}